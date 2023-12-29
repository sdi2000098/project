import numpy as np
import struct
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras.layers import Input, Conv2D, MaxPooling2D, UpSampling2D, Flatten, Reshape, Dense, Dropout, BatchNormalization,GlobalAveragePooling2D
from tensorflow.keras.models import Model
import optuna

def read_mnist_images(filename):
    with open(filename, 'rb') as file:
        magic, num_images, rows, cols = struct.unpack(">IIII", file.read(16))
        print(f"Number of images: {num_images}")
        print(f"Number of rows: {rows}")
        print(f"Number of columns: {cols}")

        image_data = np.fromfile(file, dtype=np.uint8)
        images = image_data.reshape(num_images, rows, cols)
    return images, num_images, rows, cols

# Usage
def create_autoencoder(trial, input_dim):
    n_conv_layers = trial.suggest_int('n_conv_layers', 1, 5)
    activation = trial.suggest_categorical('activation', ['relu', 'sigmoid', 'tanh'])
    dropout_rate = trial.suggest_float('dropout_rate', 0.0, 0.5)

    input_img = Input(shape=input_dim)
    x = input_img
    
    for i in range(n_conv_layers):  
        
        filters = trial.suggest_int(f'filters_layer_{i}', 8, 64)
        kernel_size = trial.suggest_categorical(f'kernel_size_layer_{i}', [(3, 3), (5, 5)])

        
        x = Conv2D(filters, kernel_size, activation=activation, padding='same')(x)
        x = MaxPooling2D((2, 2), padding='same')(x)
        x = BatchNormalization()(x)
        x = Dropout(dropout_rate)(x)

    x = GlobalAveragePooling2D()(x)

    # Dense layer
    num_neurons = trial.suggest_int('n_units_dense', 32, 128)
    x = Dense(num_neurons, activation=activation)(x)
    x = BatchNormalization()(x)
    x = Dropout(dropout_rate)(x)

    # Decoding part
    x = Dense(7 * 7 * filters, activation=activation)(x)  # Prepare for reshaping to a 7x7 feature map
    x = Reshape((7, 7, filters))(x)

    # Upsampling steps to get back to 28x28
    for _ in range(2):  # Two times upsampling from 7x7 to 28x28
        x = Conv2D(filters, kernel_size, activation=activation, padding='same')(x)
        x = UpSampling2D((2, 2))(x)
        x = BatchNormalization()(x)
        x = Dropout(dropout_rate)(x)

    # Output layer
    decoded = Conv2D(1, kernel_size, activation='sigmoid', padding='same')(x)

    autoencoder = Model(input_img, decoded)
    return autoencoder

def objective(trial):
    filename = 'dataset.dat'
    images, num_images, rows, cols = read_mnist_images(filename)
    images = images.astype(np.float32) / 255.0
    images = np.expand_dims(images, axis=-1)
    

    X_train, X_test = train_test_split(images, test_size=0.2)

    autoencoder = create_autoencoder(trial, (rows, cols, 1))
    autoencoder.summary()
    optimizer_name = trial.suggest_categorical('optimizer', ['adam', 'sgd'])
    lr = trial.suggest_float('lr', 1e-5, 1e-1, log=True)
    if optimizer_name == 'adam':
        optimizer = tf.keras.optimizers.Adam(learning_rate=lr)
    else:  # optimizer_name == 'sgd'
        momentum = trial.suggest_float('momentum', 0.0, 0.9)
        optimizer = tf.keras.optimizers.SGD(learning_rate=lr, momentum=momentum)
    autoencoder.compile(optimizer=optimizer, loss='mean_squared_error')

    batch_size = trial.suggest_int('batch_size', 64, 512, log=True)
    epochs = trial.suggest_int('epochs', 5, 15)
    autoencoder.fit(X_train, X_train, epochs=epochs, batch_size=batch_size, shuffle=True, 
                    validation_data=(X_test, X_test), verbose=0)

    test_loss = autoencoder.evaluate(X_test, X_test, verbose=0)
    return test_loss

study = optuna.create_study(direction='minimize')
study.optimize(objective, n_trials=100)

print("Study statistics: ")
print("  Number of finished trials: ", len(study.trials))
print("  Best trial:")
trial = study.best_trial
print("    Value: ", trial.value)
print("    Params: ")
for key, value in trial.params.items():
    print(f"    {key}: {value}")

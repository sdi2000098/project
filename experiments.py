import numpy as np
import struct
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras.layers import Input, Dense, Dropout, BatchNormalization
from tensorflow.keras.models import Model
import optuna
from tensorflow.keras.regularizers import l1, l2

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
    n_layers = trial.suggest_int('n_layers', 1, 5)
    activation = trial.suggest_categorical('activation', ['relu', 'sigmoid', 'tanh'])
    dropout_rate = trial.suggest_float('dropout_rate', 0.0, 0.5)
    l1_reg = trial.suggest_float('l1_reg', 1e-5, 1e-2, log=True)
    l2_reg = trial.suggest_float('l2_reg', 1e-5, 1e-2, log=True)

    input_img = Input(shape=(input_dim,))
    x = input_img

    for i in range(n_layers):
        num_neurons = trial.suggest_int(f'n_units_l{i}', 32, 512, log=True)
        x = Dense(num_neurons, activation=activation, 
                  kernel_regularizer=tf.keras.regularizers.l1_l2(l1=l1_reg, l2=l2_reg))(x)
        x = Dropout(dropout_rate)(x)
        x = BatchNormalization()(x)

    decoded = Dense(input_dim, activation='sigmoid')(x)
    autoencoder = Model(input_img, decoded)
    return autoencoder

def objective(trial):
    filename = 'dataset.dat'
    images, num_images, rows, cols = read_mnist_images(filename)
    images = images.astype(np.float32) / 255.0
    flattened_images = images.reshape((num_images, rows*cols))

    X_train, X_test = train_test_split(flattened_images, test_size=0.2)

    autoencoder = create_autoencoder(trial, rows*cols)
    optimizer_name = trial.suggest_categorical('optimizer', ['adam', 'sgd'])
    lr = trial.suggest_float('lr', 1e-5, 1e-1, log=True)
    if optimizer_name == 'adam':
        optimizer = tf.keras.optimizers.Adam(learning_rate=lr)
    else:  # optimizer_name == 'sgd'
        momentum = trial.suggest_float('momentum', 0.0, 0.9)
        optimizer = tf.keras.optimizers.SGD(learning_rate=lr, momentum=momentum)
    autoencoder.compile(optimizer=optimizer, loss='mean_squared_error')

    batch_size = trial.suggest_int('batch_size', 32, 256, log=True)
    autoencoder.fit(X_train, X_train, epochs=50, batch_size=batch_size, shuffle=True, 
                    validation_data=(X_test, X_test), verbose=0)

    test_loss = autoencoder.evaluate(X_test, X_test, verbose=0)
    return test_loss

study = optuna.create_study(direction='minimize')
study.optimize(objective, n_trials=1000)

print("Study statistics: ")
print("  Number of finished trials: ", len(study.trials))
print("  Best trial:")
trial = study.best_trial
print("    Value: ", trial.value)
print("    Params: ")
for key, value in trial.params.items():
    print(f"    {key}: {value}")

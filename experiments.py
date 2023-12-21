import numpy as np
import struct
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras.layers import Input, Dense, Flatten, Reshape
from tensorflow.keras.models import Model

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
filename = 'dataset.dat'
images, num_images, rows, cols = read_mnist_images(filename)

print(f"Shape of images: {images.shape}")

images = images.astype(np.float32)/255.0

flattened_images = images.reshape((num_images, rows*cols))

X_train, X_test = train_test_split(flattened_images, test_size=0.2)

input_img = Input(shape=(rows*cols,))
encoded = Dense(128, activation='relu')(input_img)
encoded = Dense(64, activation='relu')(encoded)

decoded = Dense(128, activation='relu')(encoded)
decoded = Dense(rows*cols, activation='sigmoid')(decoded)

autoencoder = Model(input_img, decoded)

autoencoder.compile(optimizer='adam', loss='mean_squared_error')

autoencoder.fit(X_train, X_train, epochs=50, batch_size=256, shuffle=True, validation_data=(X_test, X_test))

test_loss = autoencoder.evaluate(X_test, X_test)
print(f"Test Loss: {test_loss}")

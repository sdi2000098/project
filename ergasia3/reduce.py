import numpy as np
import struct
import tensorflow_addons as tfa
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras.layers import Input, Conv2D, MaxPooling2D, UpSampling2D, Flatten, Reshape, Dense, Dropout, BatchNormalization,GlobalAveragePooling2D
from tensorflow.keras.models import Model
from tensorflow.keras.callbacks import EarlyStopping
from tensorflow.keras.callbacks import LearningRateScheduler
import argparse
###############################################################################
#This script is used to reduce the dimensionality of the MNIST dataset and queryset as described in the assignment

def save_images_as_mnist(images, filename):     #function to save the images using the mnist format
    with open(filename, 'wb') as file:
        magic = 2051
        num_images = images.shape[0] # number of images
        rows = 1  # since each image is now a flattened array
        cols = images.shape[1]  # should be the bottleneck_size, i.e., 15
        # Writing the header
        file.write(struct.pack('>IIII', magic, num_images, rows, cols)) # > means big-endian, I means unsigned int
        

        # Writing the image data
        for image in images:
            file.write(image.tobytes())


def read_mnist_images(filename):
    with open(filename, 'rb') as file:
        magic, num_images, rows, cols = struct.unpack(">IIII", file.read(16))
        print(f"Number of images: {num_images}")
        print(f"Number of rows: {rows}")
        print(f"Number of columns: {cols}")

        image_data = np.fromfile(file, dtype=np.uint8)
        images = image_data.reshape(num_images, rows, cols)
    return images, num_images, rows, cols

def scheduler(epoch, lr):
    if epoch < 10:
        return lr
    else:
        return lr * tf.math.exp(-0.1)

def create_encoder(input_dim,bottleneck_size) :
    #Use the hyperparameters found in the autoencoder_experiments notebook
    activation = 'relu'
    dropout_rate = 0.00044795018064423126
    filters_layer_0 = 45        #First layer has 45 filters
    kernel_size_layer_0 = (5, 5)
    filters_layer_1 = 46        #Second layer has 46 filters
    kernel_size_layer_1 = (5, 5)
    
    input_img = Input(shape=input_dim)
    x = input_img
    
    x = Conv2D(filters_layer_0, kernel_size_layer_0, activation=activation, padding='same')(x)      #First convolutional layer
    x = MaxPooling2D((2, 2),strides=2, padding='valid')(x)
    x = BatchNormalization()(x)
    x = Dropout(dropout_rate)(x)
    
    x = Conv2D(filters_layer_1, kernel_size_layer_1, activation=activation, padding='same')(x)      #Second convolutional layer
    x = MaxPooling2D((2, 2),strides=2, padding='valid')(x)
    x = BatchNormalization()(x)
    x = Dropout(dropout_rate)(x)
    
    x = Flatten()(x)        #Flatten the output of the convolutional layers
    x = Dense(bottleneck_size, activation=activation)(x)       #Bottleneck layer
    x = BatchNormalization()(x)
    x = Dropout(dropout_rate)(x)
    
    encoder = Model(input_img, x)
    
    return encoder
    
def main():

    parser = argparse.ArgumentParser(description='Process MNIST images.')
    parser.add_argument('-d', '--dataset', type=str, required=True, help='Path to the dataset file')
    parser.add_argument('-q', '--queryset', type=str, required=True, help='Path to the query file')
    parser.add_argument('-od', '--output_dataset', type=str, required=True, help='Output path for the encoded dataset')
    parser.add_argument('-oq', '--output_query', type=str, required=True, help='Output path for the encoded query')

    args = parser.parse_args()


    images, num_images, rows, cols = read_mnist_images(args.dataset)
    images = images.astype(np.float32) / 255.0
    images = np.expand_dims(images, axis=-1)

    bottleneck_size = 15        #Use a bottleneck size of 15 this can be changed if needed
    encoder = create_encoder((rows, cols, 1), bottleneck_size)
    encoded_images = encoder.predict(images)    #Encode the images
    normalized_images = ((encoded_images + 1) / 2) * 255      #Normalize the images back to the 0-255 range
    normalized_images = tf.cast(normalized_images, tf.uint8)      #Cast the images to uint8
    save_images_as_mnist(normalized_images.numpy(), args.output_dataset)     #Save the images using the mnist format


    images, num_images, rows, cols = read_mnist_images(args.queryset)
    images = images.astype(np.float32) / 255.0
    images = np.expand_dims(images, axis=-1)

    encoded_images = encoder.predict(images)    #Same for the query set
    normalized_images = ((encoded_images + 1) / 2) * 255
    normalized_images = tf.cast(normalized_images, tf.uint8)
    save_images_as_mnist(normalized_images.numpy(), args.output_query)

if __name__ == '__main__':
    main()
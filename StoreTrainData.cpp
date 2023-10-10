#include "StoreTrainData.h"

#include <iostream>

using namespace std;

Image :: Image(){
    NumberOfPixels = 0;
    Pixels = NULL;
}

Image :: ~Image(){
    free(Pixels);
}

void Image :: Insert(uint8_t NewByte){
    Pixels = (uint8_t *)realloc(Pixels,++NumberOfPixels*sizeof(uint8_t));
    Pixels[NumberOfPixels-1] = NewByte;
}

void Image :: Display(void){
    for (int i = 0 ; i < NumberOfPixels ; i ++)
        cout << unsigned(Pixels[i]) << " ,";
    cout <<"]\n\n\n";
}

TrainStore :: TrainStore(){
    NumberOfImages = 0;
    AllImages = NULL;
}

TrainStore :: ~TrainStore(){
    for (int i = 0;i<NumberOfImages;i++)
        delete AllImages[i];
    free(AllImages);
}

void TrainStore :: Insert(Image * NewImage){
    AllImages = (Image **)realloc(AllImages,++NumberOfImages*sizeof(Image *));
    AllImages[NumberOfImages-1] = NewImage;
}

void TrainStore :: Display(void){
    for (int i = 0;i<5;i++)
        AllImages[i]->Display();
}
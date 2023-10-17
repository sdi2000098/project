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

void Image :: SetChecked(void){
    checked = true;
}

void Image :: SetUnchecked(void){
    checked = false;
}

bool Image :: GetChecked(void){
    return checked;
}
uint8_t * Image :: GetRepresenation(void){
    return Pixels;
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

uint8_t * TrainStore :: GetRepresenation(int Position){
    return AllImages[Position]->GetRepresenation();
}

void TrainStore :: SetChecked(int Position){
    AllImages[Position]->SetChecked();
}

void TrainStore :: SetUnchecked(int Position){
    AllImages[Position]->SetUnchecked();
}

bool TrainStore :: GetChecked(int Position){
    return AllImages[Position]->GetChecked();
}
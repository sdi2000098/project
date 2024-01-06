#include "StoreQueryData.h"

#include <iostream>

QueryStore :: QueryStore(){
    NumberOfImages = 0;
    AllImages = NULL;
}

QueryStore :: ~QueryStore(){
    for (int i = 0;i<NumberOfImages;i++)
        delete AllImages[i];
    free(AllImages);
}

void QueryStore :: Insert(Image * NewImage){
    //Allocate memory for the new image and store the pointer to it
    AllImages = (Image **)realloc(AllImages,++NumberOfImages*sizeof(Image *));
    AllImages[NumberOfImages-1] = NewImage;
}


uint8_t * QueryStore :: GetRepresentation(int Position){
    return AllImages[Position]->GetRepresenation();
}
#include <iostream>
#include "ReadQueryData.h"
#include <fstream>
#include "StoreQueryData.h"


using namespace std;
static QueryStore * MyQueryData = NULL;
static int QueryNumber;

void ReadQueryData(string path){
    ifstream file(path,ios::binary);
    if (file.is_open()){
        int Magic;
        int NumberOfImages;
        int Rows;
        int Columns;
        file.read((char*)&Magic,sizeof(Magic)); 
        Magic= reverseInt(Magic);
        file.read((char*)&NumberOfImages,sizeof(NumberOfImages));
        NumberOfImages= reverseInt(NumberOfImages);
        QueryNumber = NumberOfImages;
        file.read((char*)&Rows,sizeof(Rows));
        Rows= reverseInt(Rows);
        file.read((char*)&Columns,sizeof(Columns));
        Columns= reverseInt(Columns);
        if (MyQueryData != NULL)
            delete MyQueryData;
        MyQueryData = new QueryStore;
        for (int i = 0 ; i < NumberOfImages; i++){
            Image * ImageToInsert = new Image;
            for (int j = 0 ; j < Columns*Rows ; j++){
                uint8_t temp = 0;
                file.read((char*)&temp,sizeof(temp));
                ImageToInsert->Insert(temp);
            }
            MyQueryData->Insert(ImageToInsert);
        }
    }
    else
        cout << "Could not open file " << path << "\n";

}

void DisplayQueryData(int Pos){
    MyQueryData->Display(Pos);
}

int GetQueryNumber(void){
    return QueryNumber;
}

uint8_t *  GetQueryRepresentation(int Position){
    return MyQueryData->GetRepresentation(Position);
}
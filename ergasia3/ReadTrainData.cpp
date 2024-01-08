#include <iostream>
#include "ReadTrainData.h"
#include <fstream>
#include "StoreTrainData.h"
#define ERROR -1

using namespace std;
static TrainStore * MyTrainData = NULL;
static int TrainNumber;
static int DIMENSION ;;
int reverseInt (int x)                      //We need to change endian representation
{
    uint8_t Byte1, Byte2, Byte3, Byte4;     //Store each byte of number

    Byte1 = x & 255;                        //Least significant byte
    Byte2 = (x >> 8) & 255;
    Byte3 = (x >> 16) & 255;
    Byte4 = (x >> 24) & 255;                //Most Significant byte

    return ((int)Byte1 << 24) + ((int)Byte2 << 16) + ((int)Byte3 << 8) + Byte4;         //Change the order of bytes
}

int ReadTrainData(string path){
    //Same procedure as query data reading
    ifstream file(path,ios::binary);
    if (file.is_open()){
        int Magic;
        int NumberOfImages;
        int Rows;
        int Columns;
        file.read((char*)&Magic,sizeof(Magic)); 
        Magic= reverseInt(Magic);
        file.read((char*)&NumberOfImages,sizeof(NumberOfImages));
        NumberOfImages = reverseInt(NumberOfImages);
        TrainNumber = NumberOfImages;
        TrainNumber = 3000;
        file.read((char*)&Rows,sizeof(Rows));
        Rows= reverseInt(Rows);
        file.read((char*)&Columns,sizeof(Columns));
        Columns= reverseInt(Columns);
        DIMENSION = Rows*Columns;
        //Print info of file
        if (MyTrainData != NULL)
            delete MyTrainData;
        MyTrainData = new TrainStore;
        for (int i = 0 ; i < TrainNumber; i++){
            Image * ImageToInsert = new Image;
            for (int j = 0 ; j < Columns*Rows ; j++){
                uint8_t temp = 0;
                file.read((char*)&temp,sizeof(temp));
                ImageToInsert->Insert(temp);
            }
            MyTrainData->Insert(ImageToInsert);
        }
    }
    else{
        cout << "Could not open file " << path << "\n";
        return ERROR;
    }
    file.close();
    return 0;
}    


int GetTrainNumber(void){
    return TrainNumber;
}

uint8_t * GetRepresenation(int Position){
    return MyTrainData->GetRepresenation(Position);
}

void SetChecked(int Position){
    MyTrainData->SetChecked(Position);
}
void SetUnchecked(int Position){
    MyTrainData->SetUnchecked(Position);
}

bool GetChecked (int Position){
    return MyTrainData->GetChecked(Position);
}

void SetCluster(int Position,int NewCluster){
    MyTrainData->SetCluster(Position,NewCluster);
}
int GetCluster(int Position){
    return MyTrainData->GetCluster(Position);
}

void DeleteTrain(void){
    delete MyTrainData;
}

void SetAllUnchecked(void){
    MyTrainData->SetAllUnchecked();
}

int GetDimension(void){
    return DIMENSION;
}
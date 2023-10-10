#include <iostream>
#include "ReadTrainData.h"
#include <fstream>
#include "StoreTrainData.h"


using namespace std;
static TrainStore * MyTrainData = NULL;
int reverseInt (int x)                      //We need to change endian representation
{
    uint8_t Byte1, Byte2, Byte3, Byte4;     //Store each byte of number

    Byte1 = x & 255;                        //Least significant byte
    Byte2 = (x >> 8) & 255;
    Byte3 = (x >> 16) & 255;
    Byte4 = (x >> 24) & 255;                //Most Significant byte

    return ((int)Byte1 << 24) + ((int)Byte2 << 16) + ((int)Byte3 << 8) + Byte4;         //Change the order of bytes
}

void ReadTrainData(string path){
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
        file.read((char*)&Rows,sizeof(Rows));
        Rows= reverseInt(Rows);
        file.read((char*)&Columns,sizeof(Columns));
        Columns= reverseInt(Columns);
        if (MyTrainData != NULL)
            delete MyTrainData;
        MyTrainData = new TrainStore;
        for (int i = 0 ; i < NumberOfImages; i++){
            Image * ImageToInsert = new Image;
            for (int j = 0 ; j < Columns*Rows ; j++){
                uint8_t temp = 0;
                file.read((char*)&temp,sizeof(temp));
                ImageToInsert->Insert(temp);
            }
            MyTrainData->Insert(ImageToInsert);
        }
    }
    else
        cout << "Could not open file " << path << "\n";

}

void DisplayTrainData(void){
    MyTrainData->Display();
}
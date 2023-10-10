#include <iostream>
#include "ReadTrainData.h"
#include <fstream>
#include "StoreTrainData.h"

using namespace std;
static TrainStore * MyTrainData = NULL;
int reverseInt (int i) 
{
    uint8_t c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
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
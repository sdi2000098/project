#include <iostream>
#include "ReadQueryData.h"
#include <fstream>
#include "StoreQueryData.h"
#define ERROR -1

using namespace std;
static QueryStore * MyQueryData = NULL;
static int QueryNumber;

//File that is responsible to read query data


int ReadQueryData(string path){
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
            //After reading each image we insert it to QuryStore
            MyQueryData->Insert(ImageToInsert);
        }
    }
    else{
        cout << "Could not open file " << path << "\n";
        return ERROR;
    }
    return 0;

}


int GetQueryNumber(void){
    return QueryNumber;
}

uint8_t *  GetQueryRepresentation(int Position){
    uint8_t * ToReturn = MyQueryData->GetRepresentation(Position);
    return ToReturn;
}
void DeleteQueries(void){
    delete MyQueryData;
}
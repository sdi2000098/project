#include "lsh.h"
#include <iostream>
#define TABLE_SIZE 5


class Bucket{
    private :
        uint8_t ** Data;
        int NumberOfData;
    public :
        Bucket(){
            Data = NULL;
            NumberOfData = 0;
        }
        ~Bucket(){
            delete []Data;
        }
        void Insert(uint8_t * NewData){
            Data = (uint8_t **)(realloc(Data,++NumberOfData*sizeof(uint8_t *)));
            Data[NumberOfData -1] = NewData;
        }
};


class HashTable {
    private : 
        Bucket ** HashBuckets;
        int Size;
        //Edw xreiazetai kapws na orisoume thn g pou einai monadikh gia ka8e hash table
    public :
        HashTable(int x){
            Size = x;
            HashBuckets = new Bucket *[Size];
        }
        ~HashTable(){
            for(int i =0;i<Size;i++)
                delete HashBuckets[i];
            delete [] HashBuckets;
        }
};


LSH :: LSH(){
    L = 0;
    k = 0;
}
LSH :: ~LSH(){}

void LSH ::SetMatrix(uint8_t * Matrix){

}
void LSH ::SetTrainData(uint8_t ** TrainData,int x){
    if (L == 0 )
        SetL(5);
    
}
void LSH ::SetL(int L){
    this->L = L;
    MyHash = new HashTable*[L];
    for (int i = 0 ; i <L ; i++)
        MyHash[i] = new HashTable(TABLE_SIZE);
}
int LSH ::NearestNeighbour(void){

}
int * LSH ::KNN(int N){

}
int * LSH ::RangeSearch(double R){

}
void LSH :: SetK(int K){
    this->k = K;
}
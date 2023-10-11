#include "lsh.h"
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
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

class hFunction {
    private:
        int dimension_;
        int window_;
        std::vector<float> v;
        std::mt19937 v_generator;
        std::normal_distribution<float> v_distribution;

        std::mt19937 t_generator;
        std::uniform_real_distribution<float> t_distribution;
    public:
        hFunction(int dimension, int window) : dimension_(dimension), window_(window){
            std::random_device rd_v;
            std::mt19937 generator(rd_v());
            std::normal_distribution<float> distribution(0.0, 1.0);

            // Generate vector v
            for (int i = 0; i < dimension; ++i) {
                v[i] = distribution(generator);
            }


            // Initialize the random number generator for 't'
            std::random_device rd_t;
            t_generator = std::mt19937(rd_t());
            t_distribution = std::uniform_real_distribution<float>(0.0, static_cast<float>(window));
        }

        int operator()(const std::vector<float>& p) {
            float dot_product = 0.0f;
            for (int i = 0; i < dimension_; ++i) {
                dot_product += p[i] * v[i];
            }
            float t = t_distribution(t_generator);
            return static_cast<int>(std::round(dot_product + t) / window_);
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
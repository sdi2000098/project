#include "lsh.h"
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#define TABLE_SIZE 5
#define M 4294967291
#define MAXSIZE 1215752202
#define DIMENSION 784
#define WINDOW 5


double Euclidean(uint8_t * x, uint8_t * y, int D){
    double ToReturn = 0;
    for (int i = 0 ; i < D;i++)
        ToReturn += pow(x[i] + y[i] , 2);
    
    return sqrt(ToReturn);
}

class Bucket{
    private :
        uint8_t * Data;
        int ID,position;
        Bucket * NextBucket;
    public :
        Bucket(){
            Data = NULL;
            NextBucket = NULL;
            ID = -1;
        }
        ~Bucket(){}
        void Insert(uint8_t * NewData,int NewID,int position){
            if (NextBucket == NULL)
                NextBucket = new Bucket();
            if (ID == -1){
                position = position;
                Data = NewData;
                ID = NewID;
                return;
            }
            NextBucket->Insert(NewData,NewID,position);
        }
        double * NearestNeighbour(uint8_t * Query, int QueryId){
            double * ToReturn = new double[2];
            ToReturn[0] = MAXSIZE;
            ToReturn[1] = -1;
            if (ID == QueryId){
                ToReturn[0] = Euclidean(Query,Data,DIMENSION);
                ToReturn[1] = (double)position;
            }
            return NextBucket->InnerNearestNeighbour(Query,QueryId,ToReturn);
        }

        double * InnerNearestNeighbour(uint8_t * Query,int QueryId,double * ToReturn){
            if (NextBucket == NULL)
                return ToReturn;
            if (ID == QueryId){
                double e = Euclidean(Query,Data,DIMENSION);
                if (e < ToReturn[0]){
                    ToReturn[0] = e;
                    ToReturn[1] = (double)position;
                }
            }
            if (NextBucket == NULL)
                return ToReturn;
            return NextBucket->InnerNearestNeighbour(Query,QueryId,ToReturn);
        }
};

class hFunction {
    private:
        int dimension_;
        int window_;
        std::vector<float> v;
        float t;
        
    public:
        hFunction(int dimension, int window) : dimension_(dimension), window_(window){


            std::normal_distribution<float> v_distribution(0.0, 1.0);
            std::uniform_real_distribution<float> t_distribution(0.0,static_cast<float>(window));

            std::random_device rd_v;
            std::random_device rd_t;
            
            std::mt19937 v_generator(rd_v());
            std::mt19937 t_generator(rd_t());

            // Generate vector v
            for (int i = 0; i < dimension; ++i) {
                v.push_back(v_distribution(v_generator));
            }


            // Initialize the random number generator for 't'
            
            t = t_distribution(t_generator);
        }

        int operator()(uint8_t * p) {
            float dot_product = 0.0f;
            for (int i = 0; i < dimension_; ++i) {
                dot_product += p[i] * v[i];
            }
            return static_cast<int>(std::floor((dot_product + t) / window_));
        }

        ~hFunction(){}
};

class gFunction{
    private :
        int K;
        hFunction ** HashFunctions;
        int * r;
    
    public :
        gFunction(int K,int dimension,int window) : K(K){
            HashFunctions = new hFunction*[K];
            r = new int[K];
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(1, MAXSIZE);
            for (int i = 0 ; i < K ; i ++){
                HashFunctions[i] = new hFunction(dimension,window);
                r[i] = distrib(gen);
            }
        }
        int FindPosition(uint8_t * p ){
            int temp,temp2,accumulator=0;
            for (int i = 0 ; i < K ; i++){
                temp = r[i] % M;
                temp2 = HashFunctions[i]->operator()(p) %M;
                accumulator += (temp * temp2) % M;
            }
            return accumulator % M;
        }
        ~gFunction(){
            for (int i = 0 ; i < K;i++){
                delete HashFunctions[i];
            }
            delete [] HashFunctions;
        }

};

class HashTable {
    private : 
        Bucket ** HashBuckets;
        int TableSize;
        gFunction * MyG;
    public :
        HashTable(int K,int dimension,int window,int Size){
            TableSize = Size;
            HashBuckets = new Bucket *[TableSize];
            for (int i = 0 ; i < TableSize; i ++)
                HashBuckets[i] = new Bucket();
            MyG = new gFunction(K,dimension,window);
        }
        void Insert(uint8_t * NewData,int position){
            int ID = MyG->FindPosition(NewData);
            HashBuckets[ID%M]->Insert(NewData,ID,position);
        }
        ~HashTable(){
            for(int i =0;i<TableSize;i++)
                delete HashBuckets[i];
            delete [] HashBuckets;
        }
        double *NearestNeighbour(uint8_t * Query){
            int ID = MyG->FindPosition(Query);
            double *ToReturn;
            ToReturn = HashBuckets[ID%M]->NearestNeighbour(Query,ID);
            return ToReturn;
        }
};



LSH :: LSH(int K, int L) : L(L){
    MyHash = new HashTable * [L];
    for (int i = 0 ; i < L ; i ++)
        MyHash[i] = new HashTable(K,DIMENSION,WINDOW,TABLE_SIZE);
}

LSH :: ~LSH(){
    for (int i = 0 ; i < L ; i++)
        delete MyHash[i];
    delete [] MyHash;
}


void LSH ::SetTrainData(uint8_t ** TrainData,int NumberOfTrain){
    for (int i = 0 ; i < NumberOfTrain ; i ++)
        for (int j = 0 ; j < L ; j++)
            MyHash[j]->Insert(TrainData[i],i);
}

int LSH ::NearestNeighbour(uint8_t * Query){
    int  MinPos = -1;
    double MinSize = (double)MAXSIZE,  *ptr; 
    for (int i = 0 ; i < M; i++){
        ptr = MyHash[i]->NearestNeighbour(Query);
        if (ptr[0] < MinSize){
            MinSize = ptr[0];
            MinPos = (int)ptr[1];
        }
    }
    std::cout << "Min Size was : " << MinSize << " and Min Pos was : " << MinPos << "\n";
    return MinPos; 
}
int * LSH ::KNN(int N){
    return 0;
}
int * LSH ::RangeSearch(double R){
    return 0;
}
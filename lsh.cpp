#include "lsh.h"
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#define TABLE_SIZE 5
#define M 100
#define MAXSIZE 1215752202
#define DIMENSION 784
#define WINDOW 5
#include "ReadTrainData.h"
#include "hFunc.h"
#include <fstream>

extern const char * outputfileName;
extern std::ofstream outputFile;


double Euclidean(uint8_t * x, uint8_t * y, int D){
    double ToReturn = 0;
    for (int i = 0 ; i < D;i++)
        ToReturn += pow(x[i] - y[i] , 2);
    
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
        void Insert(uint8_t * NewData,int NewID,int NewPosition){
            if (NextBucket == NULL)
                NextBucket = new Bucket();
            if (ID == -1){
                position = NewPosition;
                Data = NewData;
                ID = NewID;
                return;
            }
            NextBucket->Insert(NewData,NewID,NewPosition);
        }
        double * InnerNearestNeighbour(uint8_t * Query,int QueryId,double * ToReturn){
            if (NextBucket == NULL)
                return ToReturn;
            if (ID == QueryId && !GetChecked(position)){
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

        double * NearestNeighbour(uint8_t * Query, int QueryId){
            double * ToReturn = new double[2];
            ToReturn[0] = MAXSIZE;
            ToReturn[1] = -1;
            if (ID == QueryId){
                ToReturn[0] = Euclidean(Query,Data,DIMENSION);
                ToReturn[1] = (double)position;
            }
            if (NextBucket == NULL)
                return ToReturn;
            return NextBucket->InnerNearestNeighbour(Query,QueryId,ToReturn);
        }

        double * AccurateInnerNearestNeighbour(uint8_t * Query,double * ToReturn){
            if (NextBucket == NULL)
                return ToReturn;
            if (!GetChecked(position)){
                double e = Euclidean(Query,Data,DIMENSION);
                if (e < ToReturn[0]){
                    ToReturn[0] = e;
                    ToReturn[1] = (double)position;
                }
            }
            if (NextBucket == NULL)
                return ToReturn;
            return NextBucket->AccurateInnerNearestNeighbour(Query,ToReturn);
        }

        double * AccurateNearestNeighbour(uint8_t * Query){
            double * ToReturn = new double[2];
                ToReturn[0] = Euclidean(Query,Data,DIMENSION);
                ToReturn[1] = (double)position;
            if (NextBucket == NULL)
                return ToReturn;
            return NextBucket->AccurateInnerNearestNeighbour(Query,ToReturn);
        }

        vector <int> RangeSearch(double R,uint8_t * Query,int QueryId){
            vector <int> ToReturn;
            if (ID == QueryId){
                double e = Euclidean(Query,Data,DIMENSION);
                if (e <=R)
                    ToReturn.push_back(position);
            }
            if (NextBucket == NULL)
                return ToReturn;
            return NextBucket->InnerRangeSearch(R,Query,QueryId,&ToReturn);
        }

        vector <int> InnerRangeSearch(double R, uint8_t * Query,int QueryId,vector <int> * ToReturn){
            if (NextBucket == NULL)
                return *ToReturn;
            if (ID == QueryId){
                double e = Euclidean(Query,Data,DIMENSION);
                if (e <=R)
                    (*ToReturn).push_back(position);
            }
            if (NextBucket == NULL)
                return *ToReturn;
            return NextBucket->InnerRangeSearch(R,Query,QueryId,ToReturn);
        }
        
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
            if ( ID < 0 )
                ID *=-1;
            HashBuckets[ID%TABLE_SIZE]->Insert(NewData,ID,position);
        }
        ~HashTable(){
            for(int i =0;i<TableSize;i++)
                delete HashBuckets[i];
            delete [] HashBuckets;
        }
        double *NearestNeighbour(uint8_t * Query){
            int ID = MyG->FindPosition(Query);
            if (ID < 0)
                ID *=-1;
            double *ToReturn;
            ToReturn = HashBuckets[ID%TableSize]->NearestNeighbour(Query,ID);
            return ToReturn;
        }

        
        double *AccurateNearestNeighbour(uint8_t * Query){

            double *ToReturn,*temp;
            for (int i = 0 ; i < TableSize;i++){
                if (i == 0 )
                    ToReturn = HashBuckets[i]->AccurateNearestNeighbour(Query);
                else{
                    temp = HashBuckets[i]->AccurateNearestNeighbour(Query);
                    if (temp[0] < ToReturn[0]){
                        ToReturn = temp;
                    }
                }
            }
            return ToReturn;
        }

        vector <int> RangeSearch(double R, uint8_t * Query){
            int ID = MyG->FindPosition(Query);
            if (ID < 0)
                ID *=-1;
            vector <int> ToReturn;
            ToReturn = HashBuckets[ID%TableSize]->RangeSearch(R,Query,ID);
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


void LSH ::SetTrainData(uint8_t * TrainData,int Position){
    for (int j = 0 ; j < L ; j++){
        MyHash[j]->Insert(TrainData,Position);
    }
}

int LSH ::NearestNeighbour(uint8_t * Query){
    int  MinPos = -1;
    double MinSize = (double)MAXSIZE,  *ptr; 
    for (int i = 0 ; i < L; i++){
        ptr = MyHash[i]->NearestNeighbour(Query);
        if (ptr[0] < MinSize){
            MinSize = ptr[0];
            MinPos = (int)ptr[1];
        }
    }
    outputFile.open(outputfileName, std::ios::app);
    outputFile <<" "<<MinPos << "\n";
    outputFile << "DistanceLSH: " << MinSize <<"\n";
    outputFile.close();
    return MinPos; 
}
void  LSH ::KNN(int K,uint8_t * Query){
    int * ToReturn = new int[K],Result;
    int * ToReturn2 = new int[K],Result2;

    for (int i = 0 ; i < K ;i++){
        outputFile.open(outputfileName, std::ios::app);
        outputFile << "Nearest neighbor-" << i+1 <<":";
        outputFile.close();
        
        Result = NearestNeighbour(Query);
        SetChecked(Result);
        ToReturn[i] = Result;
        Result2 = AccurateNearestNeighbour(Query);
        SetChecked(Result2);
        ToReturn2[i] = Result2;
    }
    for (int i = 0 ; i < K ;i ++)
    {
        SetUnchecked(ToReturn[i]);
        SetUnchecked(ToReturn2[i]);
    }
    //return ToReturn;
}

// void  LSH ::AccurateKNN(int K,uint8_t * Query){
//     int * ToReturn = new int[K],Result;
//     for (int i = 0 ; i < K ;i++){
//         cout << "Nearest neighbor-" << i <<":";
//         Result = AccurateNearestNeighbour(Query);
//         SetChecked(Result);
//         ToReturn[i] = Result;
//     }
//     for (int i = 0 ; i < K ;i ++)
//         SetUnchecked(ToReturn[i]);
//     //return ToReturn;
// }


void LSH :: Train(void){
    int limit = GetTrainNumber();
    for (int i = 0 ; i < limit ; i++){
        SetTrainData(GetRepresenation(i),i);
    }
}

int LSH ::AccurateNearestNeighbour(uint8_t * Query){
    int  MinPos = -1;
    double MinSize = (double)MAXSIZE,  *ptr; 
    for (int i = 0 ; i < L; i++){
        ptr = MyHash[i]->AccurateNearestNeighbour(Query);
        if (ptr[0] < MinSize){
            MinSize = ptr[0];
            MinPos = (int)ptr[1];
        }
    }

    outputFile.open(outputfileName, std::ios::app);
    outputFile << "DistanceTrue: " << MinSize <<"\n";
    outputFile.close();
    return MinPos; 
}

vector <int> LSH ::RangeSearch(double R,uint8_t * Query){
    vector <int> ToReturn;
    vector <int>temp;
    for (int i = 0 ; i < L ; i++){
        temp = MyHash[i]->RangeSearch(R,Query);
        if (temp.size() == 0)
            continue;
        ToReturn.insert( ToReturn.end(), temp.begin(), temp.end() );    
    }
    return ToReturn;
}
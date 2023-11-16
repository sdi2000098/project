#ifndef LSH_FLAG
    #include <iostream>
    #include <vector>
    using namespace std;
    #include <stdint.h>
    #define LSH_FLAG
    
    class HashTable;

    class LSH{
        //LSH need number of tables (L) and L hash tables
        private :
            int L;
            HashTable ** MyHash;
            void SetTrainData(uint8_t * TrainData,int Position);
        public :
            
            //KNN and accurate KNN return a vector of doubles
            //The neighbours in the vector come in pairs, first is the distance and second position
            //So the nearest neighobur's  distance is ReturnedArray[0] and position ReturnedArray[1]
            double * NearestNeighbour(uint8_t * Query);
            double * NearestNeighbour(uint8_t * Query,vector <double> * CurrentVector,int chekced);
            vector <double>  KNN(int K,uint8_t * Query);
            vector <double>  KNN(int K,uint8_t * Query,int checked);
            void Train(void);           //Train lsh based on the dataset that is stored in file StoreTrainData
            LSH(int K, int L);
            ~LSH();
            double  * AccurateNearestNeighbour(uint8_t * Query);
            vector <double>  AccurateKNN(int K,uint8_t * Query);
            std::vector <int> RangeSearch(double R,uint8_t * Query);
    };
    
#endif
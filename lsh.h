#ifndef LSH_FLAG
    #include <iostream>
    #include <vector>
    using namespace std;
    #include <stdint.h>
    #define LSH_FLAG
    
    class HashTable;

    class LSH{
        private :
            int L;
            int NumberOfTrain;
            HashTable ** MyHash;
            void SetTrainData(uint8_t * TrainData,int Position);
        public :
            
            int NearestNeighbour(uint8_t * Query);
            void  KNN(int K,uint8_t * Query);
            void Train(void);
            LSH(int K, int L);
            ~LSH();
            int  AccurateNearestNeighbour(uint8_t * Query);
            void  AccurateKNN(int K,uint8_t * Query);
            std::vector <int> RangeSearch(double R,uint8_t * Query);
    };
    
#endif
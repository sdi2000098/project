#ifndef LSH_FLAG
    #include <stdint.h>
    #define LSH_FLAG
    
    class HashTable;

    class LSH{
        private :
            int L;
            int NumberOfTrain;
            HashTable ** MyHash;
        public :
            void SetTrainData(uint8_t * TrainData,int Position);
            int NearestNeighbour(uint8_t * Query);
            int * KNN(int K,uint8_t * Query);
            int * RangeSearch(double R);
            void Train(void);
            LSH(int K, int L);
            ~LSH();
            int  AccurateNearestNeighbour(uint8_t * Query);
            int * AccurateKNN(int K,uint8_t * Query);
    };
    
#endif
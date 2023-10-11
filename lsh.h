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
            void SetTrainData(uint8_t ** TrainData,int NumberOfTrain);
            int NearestNeighbour(uint8_t * Query);
            int * KNN(int N);
            int * RangeSearch(double R);

            LSH(int K, int L);
            ~LSH();
    };
    
#endif
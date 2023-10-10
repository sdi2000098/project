#ifndef LSH_FLAG
    #include <stdint.h>
    #define LSH_FLAG
    
    class HashTable;

    class LSH{
        private :
            int k,L;
            int NumberOfTrain;
            HashTable ** MyHash;
        public :
            void SetMatrix(uint8_t * Matrix);
            void SetTrainData(uint8_t ** TrainData,int NumberOfTrain);
            void SetL(int L);
            void SetK(int K);
            int NearestNeighbour(void);
            int * KNN(int N);
            int * RangeSearch(double R);

            LSH();
            ~LSH();
    };
    
#endif
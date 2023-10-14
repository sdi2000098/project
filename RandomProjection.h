#ifndef RANDOM_PROJECTION
    #include <iostream>

    #define RANDOM_PROJECTION

    class RandomProjection
    {
    private:
        /* data */
    public:
        RandomProjection(/* args */);
        ~RandomProjection();
        int NearestNeighbour(uint8_t * Query);
        int * KNN(int K,uint8_t * Query);
        void Train(void);
        int  AccurateNearestNeighbour(uint8_t * Query);
        int * AccurateKNN(int K,uint8_t * Query);
        std::vector <int> RangeSearch(double R,uint8_t * Query);
    };
    
#endif


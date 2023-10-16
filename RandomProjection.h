#ifndef RANDOM_PROJECTION
    #include <iostream>
    #include <vector>
    #define RANDOM_PROJECTION

    class FunctionF;
    class HypercubePoint;

    class RandomProjection
    {
    private:
        int K,  M,  Probes;
        FunctionF ** MyF;
        void SetTrainData(uint8_t * TrainData,int i);
        HypercubePoint * TrainData, * LastInsert;
    public:
        //Probes, hamiltonian distance
        RandomProjection(int k,int m,int probes);
        ~RandomProjection();
        int NearestNeighbour(uint8_t * Query,int Hamming);
        int * KNN(int K,uint8_t * Query);
        void Train(void);
        int  AccurateNearestNeighbour(uint8_t * Query);
        int * AccurateKNN(int K,uint8_t * Query);
        std::vector <int> RangeSearch(double R,uint8_t * Query);
    };
    
#endif


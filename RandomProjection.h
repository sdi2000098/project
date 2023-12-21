#ifndef RANDOM_PROJECTION
    #include <iostream>
    #include <vector>  
    #define RANDOM_PROJECTION

    class FunctionF;
    class HypercubePoint;

    class RandomProjection
    {
    private:
        //Class for random projection, the functions for nearest neighhbors may be called the same way that
        //are called from LSH
        //RandomProjection needs to store the dimension of the projection (K), M and probes
        //Also creates and stores K fFunction that each one creates an LSH function
        //Finally it stores as HypercubePoint (class defined in cpp) the Train data which follow the
        //structure of a list, in order to have O(1) insertion we also store a pointer to the last data point
        int K,  M,  Probes;
        FunctionF ** MyF;
        void SetTrainData(uint8_t * TrainData,int i);
        HypercubePoint * TrainData, * LastInsert;
    public:
        //Probes, hamiltonian distance
        RandomProjection(int k,int m,int probes);
        ~RandomProjection();
        double * NearestNeighbour(uint8_t * Query,long long unsigned int Hamming);
        std::vector <double> KNN(int K,uint8_t * Query); 
        void Train(void);
        double  *   AccurateNearestNeighbour(uint8_t * Query);
        std::vector <double> AccurateKNN(int K,uint8_t * Query); 
        std::vector <int> RangeSearch(double R,uint8_t * Query); //
    };
    
#endif
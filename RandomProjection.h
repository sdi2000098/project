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
        double * NearestNeighbour(uint8_t * Query,int Hamming);
        std::vector <double> KNN(int K,uint8_t * Query); // Use std::vector here
        void Train(void);
        double  *   AccurateNearestNeighbour(uint8_t * Query);
        std::vector <double> AccurateKNN(int K,uint8_t * Query); // Use std::vector here
        std::vector <int> RangeSearch(double R,uint8_t * Query); // Use std::vector here
    };
    
#endif
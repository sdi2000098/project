#include "kmeans.h"
#include "ReadTrainData.h"
#include <random>
#include <cstring>
#include "hFunc.h"
#define TABLE_SIZE 5
#define MAXSIZE 1215752202
#define DIMENSION 784
#define WINDOW 5
#define ERROR -1

KMeans :: KMeans(int K_,char * Method, int KLSH,int L,int Kcube, int M, int probes){
    K = K_;
    Centroids = new uint8_t *[K];
    random_device rd;  
    mt19937 gen(rd()); 
    uniform_int_distribution<> distrib(0, GetTrainNumber());
    int NewCentroidIndex = distrib(gen);
    Centroids[0] = GetRepresenation(NewCentroidIndex);
    SetChecked(NewCentroidIndex);
    if (strcmp(Method,"Classic") == 0 )
        Lloyds(Centroids,K);
    else if (strcmp(Method,"LSH") == 0 )
        MyLSH(Centroids,K,KLSH,L);
    else if (strcmp(Method,"Hypercube") == 0 )
        MyCube(Centroids,K,Kcube,M,probes);
    else
        cout << "No such method" << Method << "\n";
}

void KMeans :: Lloyds(uint8_t ** Centroids, int K){
    double * AllEuclideans = new double[GetTrainNumber()];
    double Max = -1;
    for (int NumberOfCentroids =1;NumberOfCentroids < K;NumberOfCentroids++){
        for (int i = 0 ; i < GetTrainNumber() ;i++){
            if (GetChecked(i)){
                AllEuclideans[i] = 0;
                continue;
            }
            double Min = (double)MAXSIZE;
            for ( int j = 0 ; j < NumberOfCentroids ;j++){
                double e = Euclidean(Centroids[j],GetRepresenation(i),DIMENSION);
                if (e < Min)
                    Min = e;
                else if (e > Max)
                    Max = e;
            }
            AllEuclideans[i] = Min;
        }
        double Accumulator = 0;
        for (int i = 0 ; i < GetTrainNumber() ; i ++){
            AllEuclideans[i] /= Max;
            Accumulator += AllEuclideans[i];
        }
        std::uniform_real_distribution<float> t_distribution(0.0,Accumulator);
        std::random_device rd_t;
        std::mt19937 t_generator(rd_t());
        double Result = t_distribution(t_generator);
        double temp = 0,temp2;
        for (int i = 0 ; i < GetTrainNumber();i++){
            if (GetChecked(i))
                continue;
            temp2 = temp2 + AllEuclideans[i];
            if (Result >= temp && Result <= temp2){
                SetChecked(i);
                Centroids[NumberOfCentroids] = GetRepresenation(i);
            }
        }
    }
}
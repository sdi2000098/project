#include <iostream>
#include "ReadTrainData.h"
#include "kmeans.h"
#include <cstring>

int main (void){
    int K = 10,L =10 , KLSH = 4 ,Kcube = 3,M = 20, probes =2;
    ReadTrainData("dataset.dat");
    char * x = new char[sizeof("Classic")];
    strcpy(x,"Classic");
    KMeans * MyCluster = new KMeans(K,x,KLSH,L,Kcube,M,probes);
}
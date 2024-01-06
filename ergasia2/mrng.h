#ifndef MRNG
     #define MRNG

    #define POSITION 0
    #define DISTANCE 1
    #define CHECKED 2    
    //Checked is used in GenericGraphSearch to check if a node has been checked already
    // It is the index of the checked variable in the double array in GenericGraphSearch
    #define CANDITATES 20
    #define KLSH 5
    #define LLSH 5
    #define ERROR -1
    #include <vector>
    #include <iostream>
    #include "ReadTrainData.h"
    #include <algorithm>
    #include <cmath>
    #include "hFunc.h"
    #include <random>
    #include "lsh.h"
    using namespace std;

    struct GraphMRNG;
    struct GraphMRNG* createGraphMRNG(int vertices);
    void addEdge(struct GraphMRNG* graph, int src, double * NewNeighbor);
    double * GetEdge(struct GraphMRNG* graph, int src,int Position);
    int EdgesNumber(struct GraphMRNG* graph, int src);
    vector <double *> GenericGraphSearch(GraphMRNG * graph,int p, uint8_t * q,int L,int N);
    void CreateMrng(GraphMRNG * graph);
    int FindNavigating(void);
    void DeleteGraph(GraphMRNG *graph);
#endif
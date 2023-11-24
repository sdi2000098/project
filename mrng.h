#ifndef MRNG
     #define MRNG

    #define POSITION 0
    #define DISTANCE 1
    #define CHECKED 2
    #define DIMENSION 784
    #define CANDITATES 50
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

    struct Graph;
    struct Graph* createGraph(int vertices);
    void addEdge(struct Graph* graph, int src, double * NewNeighbor);
    double * GetEdge(struct Graph* graph, int src,int Position);
    int EdgesNumber(struct Graph* graph, int src);
    void PrintGraph(Graph * graph);
    double * FindTrue(uint8_t * Query,int N);
    vector <double *> GenericGraphSearch(Graph * graph,int p, uint8_t * q,int L,int N);
    void CreateMrng(Graph * graph);
    int FindNavigating(void);
    void DeleteGraph(Graph *graph);
#endif
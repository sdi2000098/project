#ifndef GNN
     #define GNN


    #define ERROR -1
    #define KLSH 5
    #define LLSH 5
    #define DIMENSION 784
    #define POSITION 0
    #define DISTANCE 1
    #include "ReadTrainData.h"
    #include <iostream>
    #include "lsh.h"
    #include <random>
    #include <algorithm>
    #include "hFunc.h"
    using namespace std;

    struct Graph* createGraph(int vertices,int NewK);
    void addEdge(struct Graph* graph, int src, int dest,int Position);
    void printGraph(struct Graph* graph);
    double * FindTrue(uint8_t * Query,int N);
    int NearestNeighbor(vector<double *> *N, uint8_t * Query, vector <double *> * S);
    vector<double *> GNNS(struct Graph * graph, uint8_t * Query, int R, int T, int E,int L);
    void CreateGnn(Graph * graph,int k);
    void DeleteGraph(Graph *graph);
#endif
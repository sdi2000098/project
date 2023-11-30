#include "gnn.h"

// Structure to represent the graph
struct GraphGNN {
    int vertices,k;
    double *** adjList; // Array of adjacency lists (a table of pointers to adjacency list where every adjacency list is a 2d array)
};

// Function to create a graph with a given number of vertices
struct GraphGNN* createGraphGNN(int vertices,int NewK) {

    struct GraphGNN* graph = (struct GraphGNN*)malloc(sizeof(struct GraphGNN));
    graph->vertices = vertices;
    graph->k = NewK;
    // Create an array of adjacency lists
    graph->adjList = (double ***)malloc(vertices * sizeof(double **));

    // Initialize each adjacency list 
    for (int i = 0; i < vertices; ++i) {
        graph->adjList[i] = (double**)malloc(NewK * sizeof(double*));
        for (int j = 0 ; j < NewK ; j ++){
            graph->adjList[i][j] = (double*)malloc(sizeof(double)*2);
            graph->adjList[i][j][POSITION] = (double) ERROR;
            graph->adjList[i][j][DISTANCE] = (double) ERROR;
        }
    }

    return graph;
}

// Function to add an edge to the graph
void addEdge(struct GraphGNN* graph, int src, int dest,int Position) {
    graph->adjList[src][Position][POSITION] = (double)dest;
}

double * FindTrue(uint8_t * Query,int N){ //find true nearest neighbors of query
    double * ToReturn = new double [N];
    for (int n = 0 ; n < N; n++){

        double minDistance = numeric_limits<double>::max();
        for (int i = 0 ; i < GetTrainNumber() ;i ++){
            double e = Euclidean(GetRepresenation(i),Query,DIMENSION);
            if ( e < minDistance){
                bool flag = false;
                for (int j = 0 ; j < n ; j++){
                    if (ToReturn[j] == e){
                        flag = true;
                        break;
                    }
                }
                if (flag)
                    continue;
                minDistance = e ;
            }
        }
        ToReturn[n] = minDistance;

    }
    return ToReturn;
}


int NearestNeighbor(vector<double *> *N, uint8_t * Query, vector <double *> * S){
    
    if ((*N).empty()) {
        // Handle the case when the vector is empty
        cerr << "Error: Vector N is empty." << endl;
        return -1; // or any other appropriate value indicating an error
    }
    
    int nearestIndex = -1;
    double minDistance = numeric_limits<double>::max(); // Initialize to a large value
    
    for (size_t i = 0; i < N->size(); i++) {
        if ((*N)[i][POSITION]==ERROR)
            break;
        bool flag = false;
        for (int j = 0 ; j < (int)S->size() ; j ++ ){
            if ((*S)[j][POSITION] == (*N)[i][POSITION]){
                flag = true;
                break;
            }
        }
        if (flag)
            continue;
        
        uint8_t* x= GetRepresenation((*N)[i][POSITION]);
        double distance = Euclidean(x, Query, DIMENSION);
        (*S).push_back((*N)[i]);
        (*N)[i][DISTANCE] = distance;
        if ((*N)[i][DISTANCE] < minDistance) {
            minDistance = (*N)[i][DISTANCE];
            nearestIndex = (int)(*N)[i][POSITION];
        }
    }
    

    return nearestIndex; //neareastIndex = new Yt (its the point in adjlist of previous Yt that is closest to query )
}


bool compareDistances(const double* a, const double* b) {
    return a[DISTANCE] < b[DISTANCE];
}
// Function to perform the GNNS algorithm
vector<double *> GNNS(struct GraphGNN * graph, uint8_t * Query, int R, int T, int E,int L) {
    vector <double *>  S;
    for (int r = 0; r < R; ++r) {

        // Seed the random number generator
        random_device rd;
        mt19937 gen(rd());

        // Generate random x and y coordinates
        uniform_int_distribution<int> distributionX(0, GetTrainNumber());
        int Yt = distributionX(gen);
        
        for (int t = 0; t < T; ++t) {
            // S = S ∪ N(Yt−1, E, G)
            vector<double *> N(graph->adjList[Yt], graph->adjList[Yt] + E);
            Yt = NearestNeighbor(&N, Query,&S);
            // Yt = arg minY∈N(Yt−1,E,G) δ(Y, Q)
            
            if (Yt == -1)
                break;
        }
    }
     
    // Sort the distances in S
    sort(S.begin(), S.end(),compareDistances);
    
    //Return L points in S with smallest distances.
    vector <double * > result(S.begin(), S.begin() + L);

    return result;
}

void CreateGnn(GraphGNN * graph,int k){
    LSH * MyLsh = new LSH(KLSH,LLSH); //lsh creation
    MyLsh->Train();
    int limit = GetTrainNumber();
    vector <double> KNNResult; 
    for (int i = 0 ; i < limit ; i++){ //for every image get its nearest neighbohrs and put them in an adjacency list
            KNNResult = MyLsh->KNN(k,GetRepresenation(i),i);
            for (int j = 0 ; j < 2*k ; j+=2 ){
                if ( j < (int)KNNResult.size() && KNNResult[j+1] != -1 )
                    addEdge(graph, i,(int) KNNResult[j+1],j/2);
                else
                    addEdge(graph, i, ERROR,j/2);
            }
    }
    delete MyLsh;
}

void DeleteGraph(GraphGNN *graph){ //free the gnn graph
    for (int i = 0 ; i < graph->vertices;i++){
        for (int j = 0 ; j < graph->k;j++)
            free(graph->adjList[i][j]);
        free(graph->adjList[i]);
    }
    free(graph->adjList);
    free(graph);
}
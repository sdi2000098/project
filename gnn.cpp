#include "gnn.h"

// Structure to represent the graph
struct GraphGNN {
    int vertices,k;
    int ** adjList; 
    // Array of adjacency lists (a table of pointers to adjacency list where every adjacency list is an array to int)
    //adjList[i] is a pointer to an array of ints (adjacency list of vertex i i.e. neighbors of vertex i)
};

// Function to create a graph with a given number of vertices
struct GraphGNN* createGraphGNN(int vertices,int NewK) {

    struct GraphGNN* graph = (struct GraphGNN*)malloc(sizeof(struct GraphGNN));
    graph->vertices = vertices;
    graph->k = NewK;
    // Create an array of adjacency lists
    graph->adjList = (int  **)malloc(vertices * sizeof(int *));

    // Initialize each adjacency list 
    for (int i = 0; i < vertices; ++i) {
        graph->adjList[i] = (int*)malloc(NewK * sizeof(int));
        for (int j = 0 ; j < NewK ; j ++){
            graph->adjList[i][j] = ERROR;
        }
    }
    return graph;
}

// Function to add an edge to the graph, i.e., add a neighbor to the adjacency list
void addEdge(struct GraphGNN* graph, int src, int dest,int Position) {
    graph->adjList[src][Position] = dest;
}

double * FindTrue(uint8_t * Query,int N){ //find true nearest neighbors of query
    double * ToReturn = new double [N];
    for (int n = 0 ; n < N; n++){

        double minDistance = numeric_limits<double>::max();
        for (int i = 0 ; i < GetTrainNumber() ;i ++){ //for all images
            double e = Euclidean(GetRepresenation(i),Query,DIMENSION); //distance of image and query
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
                minDistance = e ; //find min distance 
            }
        }
        ToReturn[n] = minDistance; //put it in the array

    }
    return ToReturn; //return the array of n nearest neighbors
}


int NearestNeighbor(vector<int > *N, uint8_t * Query, vector <double *> * S){
    
    if ((*N).empty()) {
        // Handle the case when the vector is empty
        cerr << "Error: Vector N is empty." << endl;
        return ERROR; 
    }
    
    int nearestIndex = -1;
    double minDistance = numeric_limits<double>::max(); // Initialize to a large value
    
    for (size_t i = 0; i < N->size(); i++) {
        if ((*N)[i]==ERROR)
            break;
        bool flag = false;
        for (int j = 0 ; j < (int)S->size() ; j ++ ){
            //check if its already in s
            if ((*S)[j][POSITION] == (*N)[i]){ //if its already in s
                flag = true;
                break;
            }
        }
        if (flag)   //if its already in s
            continue;
        
        uint8_t* x= GetRepresenation((*N)[i]); //get the representation of the point 
        double distance = Euclidean(x, Query, DIMENSION); //find distance between x and query
        double * ToPush = new double[2];       //This is the point that will be pushed in S
        ToPush[POSITION] = (double)(*N)[i]; //its position in the dataset
        ToPush[DISTANCE] = distance;    //its distance from query
        (*S).push_back(ToPush); //push it in S  
        if (distance < minDistance) { //find closest distance to query
            minDistance = distance;     //update min distance
            nearestIndex = (*N)[i];     //update nearest index
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
    for (int r = 0; r < R; ++r) {       // Repeat R times

        // Seed the random number generator
        random_device rd;
        mt19937 gen(rd());

        // Generate random x and y coordinates
        uniform_int_distribution<int> distributionX(0, GetTrainNumber());
        int Yt = distributionX(gen);        // Yt = random point in G
        
        for (int t = 0; t < T; ++t) {      
            // S = S ∪ N(Yt−1, E, G)
            vector<int > N(graph->adjList[Yt], graph->adjList[Yt] + E);     // N = Neighbors of Yt (expansions)
            Yt = NearestNeighbor(&N, Query,&S);
            // Yt = arg minY∈N(Yt−1,E,G) δ(Y, Q)
            
            if (Yt == -1)
                break;
        }
    }
     
    // Sort the distances in S
    sort(S.begin(), S.end(),compareDistances);
    for (size_t i = L; i < S.size(); ++i) {
        delete[] S[i];
    }
    //Return L points in S with smallest distances.
    vector <double * > result(S.begin(), S.begin() + L);

    return result;
}

void CreateGnn(GraphGNN * graph,int k){
    LSH * MyLsh = new LSH(KLSH,LLSH); //lsh creation
    MyLsh->Train(); //train lsh
    int limit = GetTrainNumber(); //get number of images
    vector <double> KNNResult; 
    for (int i = 0 ; i < limit ; i++){ //for every image get its nearest neighbohrs and put them in an adjacency list
            KNNResult = MyLsh->KNN(k,GetRepresenation(i),i); //find nearest neighbors
            for (int j = 0 ; j < 2*k ; j+=2 ){
                if ( j < (int)KNNResult.size() && KNNResult[j+1] != -1 )
                    addEdge(graph, i,(int) KNNResult[j+1],j/2); //add in adj list i the neighbor KNNResult[j+1]
                else
                    addEdge(graph, i, ERROR,j/2);
            }
    }
    delete MyLsh;
}

void DeleteGraph(GraphGNN *graph){ //free the gnn graph
    for (int i = 0 ; i < graph->vertices;i++){      // For every vertex
        free(graph->adjList[i]);        // Free its adjacency list
    }
    free(graph->adjList);       // Free the array of adjacency lists
    free(graph);            // Free the graph
}
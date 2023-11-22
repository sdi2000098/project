#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "lsh.h"
#include <fstream> // For file stream operations
#include <omp.h>
#include <random>
#include <algorithm>
#include "hFunc.h"
#define ERROR -1
#define KLSH 5
#define LLSH 5
#define DIMENSION 784
#define POSITION 0
#define DISTANCE 1

// Structure to represent the graph
struct Graph {
    int vertices,k;
    double *** adjList; // Array of adjacency lists
};


// Function to create a graph with a given number of vertices
struct Graph* createGraph(int vertices,int NewK) {

    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->vertices = vertices;
    graph->k = NewK;
    // Create an array of adjacency lists
    graph->adjList = (double ***)malloc(vertices * sizeof(double **));

    // Initialize each adjacency list as empty by default
    for (int i = 0; i < vertices; ++i) {
        graph->adjList[i] = (double**)malloc(NewK * sizeof(double*));
        for (int j = 0 ; j < NewK ; j ++){
            graph->adjList[i][j] = (double*)malloc(sizeof(double)*2);
             graph->adjList[i][j][POSITION] = ERROR;
        }
    }

    return graph;
}

// Function to add an edge to the graph
void addEdge(struct Graph* graph, int src, int dest,int Position) {
    graph->adjList[src][Position][POSITION] = (double)dest;
}

// Function to print the adjacency list representation of the graph
void printGraph(struct Graph* graph) {
    for (int i = 0; i < graph->vertices; ++i) {
        printf("Adjacency list of vertex %d: ", i);
        for(int j =0;j<graph->k;j++)
            printf("%d ",(int)graph->adjList[i][j][POSITION]);
        printf("\n");
    }
}

double * FindTrue(uint8_t * Query,int N){
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
    

    return nearestIndex;
}


bool compareDistances(const double* a, const double* b) {
    return a[DISTANCE] < b[DISTANCE];
}
// Function to perform the GNNS algorithm
vector<double *> GNNS(struct Graph * graph, uint8_t * Query, int R, int T, int E,int L) {
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
           // N=Eneighbors(graph->adjList[Yt],E);
            // Yt = arg minY∈N(Yt−1,E,G) δ(Y, Q)
            
            if (Yt == -1)
                continue;
        }
    }
     
    // Sort the distances in S
    sort(S.begin(), S.end(),compareDistances);
    
    //Return L points in S with smallest distances.
    vector <double * > result(S.begin(), S.begin() + L);

    return result;
}


int main(int argc, char* argv[]) {
    const char * outputfileName;
    ofstream outputFile;
    string inputFile , queryFile, answer;


    //Initialize default values
    

    int k=50;
    int E=30;
    double R = 1;
    int N=5;



   for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        //Check for flags and change values if needed
        if (arg == "-d" && i + 1 < argc) {
            inputFile = argv[i + 1];
            i++;
        } else if (arg == "-q" && i + 1 < argc) {
            queryFile = argv[i + 1];
            i++;
        } else if (arg == "-k" && i + 1 < argc) {
            k = stoi(argv[i + 1]);
            i++;
        } else if (arg == "-E" && i + 1 < argc) {
            E = stoi(argv[i + 1]);
            i++;
        } else if (arg == "-R" && i + 1 < argc) {
            R = stoi(argv[i + 1]);
            i++;
        } else if (arg == "-N" && i + 1 < argc) {
            N = stoi(argv[i + 1]);
            i++;
        } else if (arg == "-o" && i + 1 < argc) {
            outputfileName = argv[i + 1];
            i++;
        }
        else{
            cout << "Unexpected argument in command line\n";
            return ERROR;
        }
    }
    if (k<= 0 || E <= 0 || N <= 0 || R <=0){
        cout << "K,L,N,R need to be positive integers\n";
        return ERROR;
    }
    
    if (inputFile.empty()){         //Input file not initialized by command line
        cout << "Please insert path to dataset :\n";
        cin >> inputFile;
    }
    if (ifstream(outputfileName)) {
            remove(outputfileName);
        }
    outputFile.open(outputfileName, ios::app);
    if (!outputFile.is_open()) {
        cerr << "Error: Could not open the output file." << endl;
        return ERROR;
    }
    if ( ReadTrainData(inputFile) == ERROR)
        return ERROR;

    LSH * MyLsh = new LSH(KLSH,LLSH);
    MyLsh->Train();

    int limit = GetTrainNumber();

    

    vector <double> KNNResult; 

    struct Graph* graph = createGraph(limit,k);

    #pragma omp parallel for
    for (int i = 0 ; i < limit ; i++){
            KNNResult = MyLsh->KNN(k,GetRepresenation(i),i);
            for (int j = 0 ; j < 2*k ; j+=2 ){
                
                if ( j < (int)KNNResult.size() && KNNResult[j+1] != -1 )
                    addEdge(graph, i,(int) KNNResult[j+1],j/2);
                else
                    addEdge(graph, i, ERROR,j/2);
            }
    }

    if ( ReadQueryData(queryFile) == ERROR)
        return ERROR;

    //int limit2 = GetQueryNumber();
    int limit2 = 5;
    clock_t start, end;
    double GNNtime ,AccurateTime ;
    for (int i = 0 ; i < limit2 ; i++){
        start = clock();
        vector<double *> currentResult = GNNS(graph, GetQueryRepresentation(i), R, 50, E,N);
        end = clock();
        GNNtime = double(end - start) / double(CLOCKS_PER_SEC);
        
        start = clock();
        double * TrueNeighbor = FindTrue(GetQueryRepresentation(i),N);
        end = clock();
        AccurateTime = double(end - start) / double(CLOCKS_PER_SEC);

        

        for (int j = 0 ; j < (int)currentResult.size();j++){
            if (currentResult[j][POSITION] == ERROR){
                outputFile << "Could not fine approximate nearest neighbor" << j  <<"\n";
                continue;
            }
            if (TrueNeighbor[j] == ERROR){
                outputFile << "Could not fine accuate nearest neighbor" << j  <<"\n";
                continue;
            }
            outputFile << "Nearest neighbor-" << j+1 << " : " << currentResult[j][POSITION] << "\n";
            
            outputFile << "distanceApproximate : " << currentResult[j][DISTANCE] <<"\n";
            
            outputFile << "distanceTrue : " << TrueNeighbor[j] <<"\n";
        }
        
        outputFile << "tAverageApproximate : " << GNNtime <<"\n";
        outputFile << "tAverageTrue : " << AccurateTime << "\n\n";
        outputFile << "------------------------------------------------\n";
    }

    return 0;
}
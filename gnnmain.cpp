#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "lsh.h"
#include <fstream> // For file stream operations
#include <omp.h>
#include <random>
#include <algorithm>
#define ERROR -1
#define KLSH 5
#define LLSH 5
// // Structure to represent a node in the adjacency list
// struct Node {
//     int data;
//     struct Node* next;
// };

// Structure to represent the graph
struct Graph {
    int vertices,k;
    int ** adjList; // Array of adjacency lists
};

// // Function to create a new node
// struct Node* createNode(int data) {
//     struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
//     newNode->data = data;
//     return newNode;
// }

// Function to create a graph with a given number of vertices
struct Graph* createGraph(int vertices,int NewK) {

    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->vertices = vertices;
    graph->k = NewK;
    // Create an array of adjacency lists
    graph->adjList = (int **)malloc(vertices * sizeof(int *));

    // Initialize each adjacency list as empty by default
    for (int i = 0; i < vertices; ++i) {
        graph->adjList[i] = (int*)malloc(NewK * sizeof(int));
    }

    return graph;
}

// Function to add an edge to the graph
void addEdge(struct Graph* graph, int src, int dest,int Position) {
    // Add an edge from src to dest
    /*struct Node* newNode = createNode(dest);
    newNode->next = graph->adjList[src];
    graph->adjList[src] = newNode;*/
    graph->adjList[src][Position] = dest;

}

// Function to print the adjacency list representation of the graph
void printGraph(struct Graph* graph) {
    for (int i = 0; i < graph->vertices; ++i) {
        printf("Adjacency list of vertex %d: ", i);
        for(int j =0;j<graph->k;j++)
            printf("%d  ",graph->adjList[i][j]);
        printf("\n");
    }
}



// Function to perform the GNNS algorithm
std::vector<int> GNNS(struct Graph * graph, uint8_t * Query, int R, int T, int E) {
    std::vector<int> result;
    vector <int>  S;

    for (int r = 0; r < R; ++r) {

        // Seed the random number generator
        std::random_device rd;
        std::mt19937 gen(rd());

        // Generate random x and y coordinates
        std::uniform_int_distribution<int> distributionX(0, GetTrainNumber());
        int Yt = distributionX(gen);

        for (int t = 0; t < T; ++t) {
            // S = S ∪ N(Yt−1, E, G)
            
           // N=Eneighbors(graph->adjList[Yt],E);
            std::vector<int> N(graph->adjList[Yt], graph->adjList[Yt] + E);

            S.insert(S.end(), N.begin(), N.end());

            // Yt = arg minY∈N(Yt−1,E,G) δ(Y, Q)
            Yt = NearestNeighbor(N, Query);
        }
    }

    // Sort the distances in S
    std::sort(S.begin(), S.end());
    //Return L points in S with smallest distances.

    return result;
}


int main(int argc, char* argv[]) {
    const char * outputfileName;
    std::ofstream outputFile;
    std::string inputFile , queryFile, answer;


    //Initialize default values
    

    int k=50;
    int E=30;
    double R = 1;
    int N=1;



   for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        //Check for flags and change values if needed
        if (arg == "-d" && i + 1 < argc) {
            inputFile = argv[i + 1];
            i++;
        } else if (arg == "-q" && i + 1 < argc) {
            queryFile = argv[i + 1];
            i++;
        } else if (arg == "-k" && i + 1 < argc) {
            k = std::stoi(argv[i + 1]);
            i++;
        } else if (arg == "-E" && i + 1 < argc) {
            E = std::stoi(argv[i + 1]);
            i++;
        } else if (arg == "-R" && i + 1 < argc) {
            R = std::stoi(argv[i + 1]);
            i++;
        } else if (arg == "-N" && i + 1 < argc) {
            N = std::stoi(argv[i + 1]);
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
    if (std::ifstream(outputfileName)) {
            std::remove(outputfileName);
        }
    outputFile.open(outputfileName, std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open the output file." << std::endl;
        return ERROR;
    }
    if ( ReadTrainData(inputFile) == ERROR)
        return ERROR;

    LSH * MyLsh = new LSH(KLSH,LLSH);
    MyLsh->Train();

    int limit = GetTrainNumber();
    

    vector <double> KNNResult; 

    struct Graph* graph = createGraph(limit,k);

    clock_t start = clock(), end;
    double time;
    #pragma omp parallel for
    for (int i = 0 ; i < limit ; i++){
        fprintf(stderr,"%d\n",i);
            KNNResult = MyLsh->KNN(k,GetRepresenation(i),i);
            for (int j = 0 ; j < 2*k ; j+=2 ){
                
                if ( j < (int)KNNResult.size() && KNNResult[j+1] != -1 )
                    addEdge(graph, i, KNNResult[j+1],j/2);
                else
                    addEdge(graph, i, ERROR,j/2);
            }
    }
    end = clock();
    time = double(end - start) / double(CLOCKS_PER_SEC);
    outputFile << "time = " << time << "\n";
    outputFile.flush();
    printGraph(graph);

    int limit2 = GetQueryNumber();

    for (int i = 0 ; i < limit ; i++){
    vector<int> result = GNNS(graph, GetQueryRepresentation(i), R, 50, E);
    
    }
    return 0;
}


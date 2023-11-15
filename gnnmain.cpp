#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "lsh.h"
#include <fstream> // For file stream operations
#define ERROR -1

// Structure to represent a node in the adjacency list
struct Node {
    int data;
    struct Node* next;
};

// Structure to represent the graph
struct Graph {
    int vertices;
    struct Node** adjList; // Array of adjacency lists
};

// Function to create a new node
struct Node* createNode(int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to create a graph with a given number of vertices
struct Graph* createGraph(int vertices) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->vertices = vertices;

    // Create an array of adjacency lists
    graph->adjList = (struct Node**)malloc(vertices * sizeof(struct Node*));

    // Initialize each adjacency list as empty by default
    for (int i = 0; i < vertices; ++i) {
        graph->adjList[i] = NULL;
    }

    return graph;
}

// Function to add an edge to the graph
void addEdge(struct Graph* graph, int src, int dest) {
    // Add an edge from src to dest
    struct Node* newNode = createNode(dest);
    newNode->next = graph->adjList[src];
    graph->adjList[src] = newNode;
}

// Function to print the adjacency list representation of the graph
void printGraph(struct Graph* graph) {
    for (int i = 0; i < graph->vertices; ++i) {
        struct Node* temp = graph->adjList[i];
        printf("Adjacency list of vertex %d: ", i);
        while (temp) {
            printf("%d -> ", temp->data);
            temp = temp->next;
        }
        printf("NULL\n");
    }
}

int main(int argc, char* argv[]) {
    const char * outputfileName;
    std::ofstream outputFile;
    std::string inputFile , queryFile, answer;


    //Initialize default values
    int K = 4;
    int L = 5;
    

    int k=5;
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
            K = std::stoi(argv[i + 1]);
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

    if ( ReadTrainData(inputFile) == ERROR)
        return ERROR;

    // do{
        LSH * MyLsh = new LSH(K,L);
        MyLsh->Train();

        int limit = GetTrainNumber();

        vector <double> KNNResult; 

        // Create a graph with 5 vertices
        struct Graph* graph = createGraph(limit);

        for (int i = 0 ; i < limit ; i++){
           // cout << "Query : "<<i<<std::endl;

            KNNResult = MyLsh->KNN(k,GetRepresenation(i));

            for (int j = 0 ; j < 2*k ; j+=2 ){
                
                if ( j < (int)KNNResult.size() && KNNResult[j+1] != -1 ){
                    cout << "Nearest neighbor-"<<j/2 +1<< ": " << KNNResult[j+1] << "\n";
                    addEdge(graph, i, KNNResult[j+1]);
                }
                else
                    cout << "Could not find Nearest neighbor " << j/2 << " using aproximate KNN\n";
            }
        }
        printGraph(graph);


    //     std::cout<<"Terminate program? (y/n)\n";
    //     std::cin>>answer;
    //     if (answer=="y")
    //         exit(0);
    //     else
    //     {
    //         std::cout<<"Give queryfile\n";

    //         std::cin >> queryFile;
    //     }
    // } while(answer=="n");

    return 0;
}


#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "lsh.h"
#include <fstream> // For file stream operations
#include <random>
#include <algorithm>
#include "hFunc.h"
#include "gnn.h"
#include "mrng.h"
#include <string.h>
#include <chrono>
#include "GetTrueDistances.h"
#define GREEDY_STEPS 50 //define t
using namespace std;
int main (int argc, char* argv[]){
    const char * outputfileName = NULL;
    ofstream outputFile;
    string inputFile , queryFile,answer;
    string inputFileReduced, queryFileReduced;
    int L = 250,N = 1,k=100, E=30,R = 1, m = 0;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        //Check for flags and change values if needed
        if (arg == "-d" && i + 1 < argc) {
            inputFile = argv[i + 1];
            i++;
        } else if (arg == "-q" && i + 1 < argc) {
            queryFile = argv[i + 1];
            i++;
        } else if (arg == "-l" && i + 1 < argc) {
            L = stoi(argv[i + 1]);
            i++;
        }else if (arg == "-N" && i + 1 < argc) {
            N = stoi(argv[i + 1]);
            i++;
        } else if (arg == "-o" && i + 1 < argc) {
            outputfileName = argv[i + 1];
            i++;
        }
        else if (arg == "-k" && i + 1 < argc) {
            k = stoi(argv[i + 1]);
            i++;
        }
        else if (arg == "-E" && i + 1 < argc) {
            E = stoi(argv[i + 1]);
            i++;
        } 
        else if (arg == "-R" && i + 1 < argc) {
            R = stoi(argv[i + 1]);
            i++;
        } 
        else if (arg == "-m" && i + 1 < argc) {
            m = stoi(argv[i + 1]);
            i++;
        } else if (arg == "-dr" && i + 1 < argc) {
            inputFileReduced = argv[i + 1];
            i++;
        } else if (arg == "-qr" && i + 1 < argc) {
            queryFileReduced = argv[i + 1];
            i++;
        }
        else{
            cout << "Unexpected argument in command line\n";
            return ERROR;
        }
    }
    if (k<= 0 || E <= 0 || N <= 0 || R <=0 || L<=0){
        cout << "K,L,N,R need to be positive integers\n";
        return ERROR;
    }
    if (m != 1 && m !=2){
        cout << "M should be 1 for gnn or 2 for mrng\n";
        return  ERROR;
    }
    if (inputFile.empty()){         //Input file not initialized by command line
        cout << "Please insert path to dataset :\n";
        cin >> inputFile;
    }
    if (outputfileName == NULL){
        cout << "Please insert output file\n";
        return ERROR;
    }
    
    if ( ReadTrainData(inputFileReduced) == ERROR) //read dataset
        return ERROR;
    int limit = GetTrainNumber(),Navigating = 0; //limit = number of images
    
    if ( ReadQueryData(queryFileReduced) == ERROR) //read query file
        return ERROR;
    
    GraphGNN * graph1 = NULL;
    GraphMRNG * graph2 = NULL;

    if (m == 1){
        graph1 = createGraphGNN(limit,k);  //for gnn algorithm
        CreateGnn(graph1,k); //initialization and creation of graph
    }
    else{
        graph2 = createGraphMRNG(limit); //for mrng algorithm
        CreateMrng(graph2); //initialization and creation of graph
        Navigating = FindNavigating();
    }
    if (ifstream(outputfileName)) 
        remove(outputfileName);
    outputFile.open(outputfileName, std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open the output file." << std::endl;
        return ERROR;
    }
    int limit2 = GetQueryNumber(); //limit2 number of queries
    if (m == 1)
        outputFile << "GNN Results\n";
    else
        outputFile << "MRNG Results\n";
    double GraphSearchTime =0 ,AccurateTime =0;
    auto start = chrono::high_resolution_clock::now();
    vector <int> indexes[limit2];
    for (int i = 0 ; i < limit2;i++){ //for all queries
        vector<double *> currentResult;
        //Current result is a vector of double arrays, each array has 2 values : position in dataset, distance of point from query
        if (m == 1)
            currentResult = GNNS(graph1, GetQueryRepresentation(i), R, GREEDY_STEPS, E,N); //gnn algorithm
        else
            currentResult = GenericGraphSearch(graph2,Navigating,GetQueryRepresentation(i),L,N); //mrng algorithm
        for (int j = 0;j<N;j++)
            indexes[i].push_back((int)currentResult[j][POSITION]);
        for (int j = 0 ; j < (int)currentResult.size();j++) //free this array
                delete [] currentResult[j];
    }
    auto stop = chrono::high_resolution_clock::now();
    //Same as brute force 
    //Return to original space and calculate true distances
    //and then calculate mean approximation factor

    if (ReadTrainData(inputFile) == ERROR)
        return NULL;
    if (ReadQueryData(queryFile) == ERROR)
        return NULL;
    double * Distances;
    for (int i = 0;i<limit2;i++){
        outputFile << "Query : "<<i<<std::endl;
        Distances = GetTrueDistances(indexes[i],GetQueryRepresentation(i));
        for (int j = 0;j<N;j++){
            outputFile << "Nearest neighbor-" << j+1 <<": "<<indexes[i][j]<<endl;
            outputFile << "distance: "<<Distances[j]<<endl;
        }
        delete [] Distances;
    }
    
    chrono::duration<double> duration = stop - start;
    outputFile << "Average time : " << duration.count()/(limit2*N) << " seconds" << endl;
    double MeanApproximationFactor = 0;
    double minDistance;
    int minIndex;

    for (int i = 0 ; i < limit2 ; i ++){
        uint8_t * query = GetQueryRepresentation(i);
        SetAllUnchecked();
        for (int j = 0;j<N;j++){
            minDistance = 100000000;
            for (int k = 0;k<GetTrainNumber();k++){
                uint8_t * train = GetRepresenation(k);
                double distance = Euclidean(query,train,GetDimension());
                if (distance < minDistance && GetChecked(k) == false){
                    minDistance = distance;
                    minIndex = k;
                }
            }
            SetChecked(minIndex);
            MeanApproximationFactor += Euclidean(GetRepresenation(indexes[i][j]),query,GetDimension()) /minDistance;
        }
        
    }
    MeanApproximationFactor = MeanApproximationFactor/(limit2*N);
    outputFile << "Mean Approximation Factor: " << MeanApproximationFactor << endl;
    outputFile.close();
    
    if (m == 1)
        DeleteGraph(graph1);
    else
        DeleteGraph(graph2);

    DeleteQueries(); //free queries data
    DeleteTrain(); //free images data
    cout << "Return Value: " << MeanApproximationFactor << endl;
}
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
#include "gnn.h"
#define ERROR -1
#define KLSH 5
#define LLSH 5
#define DIMENSION 784
#define POSITION 0
#define DISTANCE 1



int main(int argc, char* argv[]) {
    const char * outputfileName= NULL;
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
    if (outputfileName == NULL){
        cout << "Please insert output file\n";
        return ERROR;
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

    

    int limit = GetTrainNumber();

    

    

    struct Graph* graph = createGraph(limit,k);

    
    CreateGnn(graph,k);

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
        delete []TrueNeighbor;
    }
    DeleteGraph(graph);
    DeleteQueries();
    DeleteTrain();
    return 0;
}
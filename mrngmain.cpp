#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>

#include <fstream> // For file stream operations
#include <omp.h>
#include <random>
#include <algorithm>
#include "hFunc.h"
#include "mrng.h"
#define ERROR -1
#define DIMENSION 784
#define POSITION 0
#define DISTANCE 1


int main (int argc, char* argv[]){
    const char * outputfileName = NULL;
    ofstream outputFile;
    string inputFile , queryFile;

    int L = 20,N = 5;

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
        else{
            cout << "Unexpected argument in command line\n";
            return ERROR;
        }
    }
    if (N <= 0 || L <=0){
        cout << "L,N need to be positive integers\n";
        return ERROR;
    }
    if (outputfileName == NULL){
        cout << "Please provide output file\n";
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
    int limit = GetTrainNumber();
    Graph * graph = createGraph(limit);
    CreateMrng(graph);
    int Navigating = FindNavigating();
    if ( ReadQueryData(queryFile) == ERROR)
        return ERROR;
    //int limit2 = GetQueryNumber();
    int limit2 = 5;
    clock_t start, end;
    double GenericSearchTime ,AccurateTime ;

    for (int i = 0 ; i < limit2;i++){
        start = clock();
        vector<double *> currentResult = GenericGraphSearch(graph,Navigating,GetQueryRepresentation(i),L,N);
        end = clock();
        GenericSearchTime = double(end - start) / double(CLOCKS_PER_SEC);
        
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
        
        outputFile << "tAverageApproximate : " << GenericSearchTime <<"\n";
        outputFile << "tAverageTrue : " << AccurateTime << "\n\n";
        outputFile << "------------------------------------------------\n";
        for (int j = 0 ; j < (int)currentResult.size();j++)
            delete [] currentResult[j];
        delete []TrueNeighbor;
    }
    DeleteGraph(graph);
    DeleteQueries();
    DeleteTrain();
}
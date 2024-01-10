#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include <fstream> // For file stream operations
#include <vector>
#include "hFunc.h"
#include "GetTrueDistances.h"
using namespace std;
#define ERROR -1
#include <chrono>

int main(int argc, char* argv[]) {
    ofstream outputFile;
    string inputFile , queryFile,outputfileName;
    int K = 5;
    for (int i = 1;i<argc;i++){
        string arg = argv[i];
        if (arg == "-d" && i + 1 < argc) {
            inputFile = argv[i + 1];
            i++;
        } else if (arg == "-q" && i + 1 < argc) {
            queryFile = argv[i + 1];
            i++;
        } else if (arg == "-o" && i + 1 < argc) {
            outputfileName = argv[i + 1];
            i++;
        }
        else if (arg == "-k" && i + 1 < argc) {
            K = stoi(argv[i + 1]);
            i++;
        } 
        else{
            cout << "Unexpected argument in command line\n";
            return ERROR;
        }
    }
    if (inputFile.empty()){         //Input file not initialized by command line
        cout << "Please insert path to dataset :\n";
        cin >> inputFile;
    }
    if (queryFile.empty()){         //Query file not initialized by command line
        cout << "Please insert path to query file :\n";
        cin >> queryFile;
    }
    if (outputfileName.empty()){    //Output file not initialized by command line
        cout << "Please insert path to output file :\n";
        cin >> outputfileName;
    }
    if (inputFile.empty() || queryFile.empty() || outputfileName.empty()){
        cout << "One or more files not initialized\n";
        return ERROR;
    }
    
    if (ReadTrainData(inputFile) == ERROR)
        return ERROR;
    if (ReadQueryData(queryFile) == ERROR)
        return ERROR;
    if (ifstream(outputfileName)){
        remove(outputfileName.c_str());
    }
    outputFile.open(outputfileName,ios::app);
    if (!outputFile.is_open()){
        cout << "Error opening output file\n";
        return ERROR;
    }
    int limit = GetQueryNumber();
    limit = 20
    ;
    double minDistance;
    int minIndex;
    //Start measuring time
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0;i<limit;i++){
        uint8_t * query = GetQueryRepresentation(i);
        SetAllUnchecked();
        outputFile << "Query : " << i << endl;
        for (int j = 0;j<K;j++){
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
            outputFile << "Nearest neighbour-" << j+1 << ": " << minIndex << endl;
            outputFile << "distance: " << minDistance << endl; 
        }   
    }
    auto stop = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = stop - start;
    outputFile << "Average time : " << duration.count()/(limit*K) << " seconds" << endl;
    outputFile.close();
    DeleteQueries();
    DeleteTrain();
    return  0;
}
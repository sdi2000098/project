#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "lsh.h"
#include <fstream> // For file stream operations
#include <chrono>
#include "GetTrueDistances.h"

#define ERROR -1



int main(int argc, char* argv[]) {
    const char * outputfileName;
    std::ofstream outputFile;
    std::string inputFile , queryFile, answer;

    string inputFileReduced, queryFileReduced;
    //Initialize default values
    int K = 4;
    int L = 5;
    int N = 1;
    double R = 10000;


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
        } else if (arg == "-L" && i + 1 < argc) {
            L = std::stoi(argv[i + 1]);
            i++;
        } else if (arg == "-o" && i + 1 < argc) {
            outputfileName = argv[i + 1];
            i++;
        } else if (arg == "-N" && i + 1 < argc) {
            N = std::stoi(argv[i + 1]);
            i++;
        } else if (arg == "-R" && i + 1 < argc) {
            R = std::stod(argv[i + 1]);
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
    if (K<= 0 || L <= 0 || N <= 0 || R <=0){
        cout << "K,L,N,R need to be positive integers\n";
        return ERROR;
    }
    
    if (inputFile.empty()){         //Input file not initialized by command line
        cout << "Please insert path to dataset :\n";
        cin >> inputFile;
    }

    if ( ReadTrainData(inputFileReduced) == ERROR) //read dataset
        return ERROR;
    if ( ReadQueryData(queryFileReduced) == ERROR) //read query file
        return ERROR;
    
    if (queryFile.empty()){
        cout << "Please insert path to query file\n";
        cin >> queryFile;
    }
    LSH * MyLsh = new LSH(K,L);
    MyLsh->Train();
    int limit = GetQueryNumber();
    limit = 20;
    // Check if the file exists and delete it if it does
    if (std::ifstream(outputfileName)) {
        std::remove(outputfileName);
    }
    outputFile.open(outputfileName, std::ios::app);
    // Check if the file is opened successfully
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open the output file." << std::endl;
        return ERROR;
    }
    vector <double> KNNResult;    
    auto start = chrono::high_resolution_clock::now();
    vector <int> indexes[limit];
    for (int i = 0 ; i < limit ; i++){
        KNNResult = MyLsh->KNN(N,GetQueryRepresentation(i));
        for (int j = 0 ; j < 2*N ; j+=2 )
            indexes[i].push_back(KNNResult[j+1]);
        
    }
    auto stop = chrono::high_resolution_clock::now();
    if (ReadTrainData(inputFile) == ERROR)
        return NULL;
    if (ReadQueryData(queryFile) == ERROR)
        return NULL;
    double * Distances;
    for (int i = 0;i<limit;i++){
        outputFile << "Query : "<<i<<std::endl;
        Distances = GetTrueDistances(indexes[i],GetQueryRepresentation(i));
        for (int j = 0;j<N;j++){
            outputFile << "Nearest neighbor-" << j+1 <<": "<<indexes[i][j]<<endl;
            outputFile << "distance: "<<Distances[j]<<endl;
        }
        delete [] Distances;
    }
    chrono::duration<double> duration = stop - start;
    outputFile << "Average time : " << duration.count()/(limit*N) << " seconds" << endl;
    double MeanApproximationFactor = 0;
    double minDistance;
    int minIndex;

    for (int i = 0 ; i < limit ; i ++){
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
    MeanApproximationFactor = MeanApproximationFactor/(limit*N);
    outputFile << "Mean Approximation Factor: " << MeanApproximationFactor << endl;
    outputFile.close();
    cout << "Return Value: " << MeanApproximationFactor << endl;
}




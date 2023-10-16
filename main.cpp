#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "lsh.h"
#include <fstream> // For file stream operations
#define ERROR -1
const char * outputfileName;
std::ofstream outputFile;


int main(int argc, char* argv[]) {

    std::string inputFile , queryFile;

    int K = 4;
    int L = 1;
    int N = 5;
    double R = 100000;


   for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

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

    if ( ReadTrainData(inputFile) == ERROR)
        return ERROR;
    
    if (queryFile.empty()){
        cout << "Please insert path to query file\n";
        cin >> queryFile;
    }
    
    if ( ReadQueryData(queryFile) == ERROR)
        return ERROR;

    LSH * MyLsh = new LSH(K,L);
    MyLsh->Train();
    
    int limit = 5;

    // Check if the file exists and delete it if it does
    if (std::ifstream(outputfileName)) {
        std::remove(outputfileName);
    }
    outputFile.open(outputfileName, std::ios::app);
    // Check if the file is opened successfully
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open the file." << std::endl;
        return ERROR;
    }
    vector <double> KNNResult, AcuurateKNNReult;    
    clock_t start, end;
    double KNNTIme = 0,AccurateKNNTime = 0;
    for (int i = 0 ; i < limit ; i++){
        
        outputFile << "Query : "<<i<<std::endl;

        start = clock();
        KNNResult = MyLsh->KNN(N,GetQueryRepresentation(i));
        end = clock();
        KNNTIme += double(end - start) / double(CLOCKS_PER_SEC);

        start = clock();
        AcuurateKNNReult = MyLsh->AccurateKNN(K,GetQueryRepresentation(i));
        end = clock();
        AccurateKNNTime += double(end - start) / double(CLOCKS_PER_SEC);

        for (int j = 0 ; j < 2*K ; j+=2 ){
            if ( j < (int)KNNResult.size()){
                outputFile << "Nearest neighbor-"<<j/2 +1<< ": " << KNNResult[j+1] << "\n";
                outputFile << "distanceLSH: " << KNNResult[j] <<"\n";
            }
            else
                outputFile << "Could not find Nearest neighbor " << j/2 << " using aproximate KNN\n";
            if (j <(int) AcuurateKNNReult.size())
                outputFile << "distanceTrue: " << AcuurateKNNReult[j] << "\n";
            else 
                outputFile << "Could not find Nearest neighbor " << j/2 << " using exhaustive KNN\n";

        }

        outputFile << "tLSH: " << KNNTIme << "\ntTrue: " << AccurateKNNTime <<"\n";

        vector <int> Range = MyLsh->RangeSearch(R,GetQueryRepresentation(i));
        outputFile << "R-near neighbors:\n";
        for (int j = 0 ; j < (int)Range.size();j++)
            outputFile << Range[j] << "\n";

        
    }

    outputFile.close();

    return 0;
}




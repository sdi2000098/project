#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "lsh.h"
#include <fstream> // For file stream operations

const char * outputfileName;
std::ofstream outputFile;

double Euclidean2(uint8_t * x, uint8_t * y, int D){
    double ToReturn = 0;
    for (int i = 0 ; i < D;i++)
        ToReturn += pow(x[i] - y[i] , 2);
    
    return sqrt(ToReturn);
}
int main(int argc, char* argv[]) {

    std::string inputFile, queryFile;

    int K = 4;
    int L = 1;
    int N = 5;
    int R = 100000;


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
    }

    ReadTrainData(inputFile);
    ReadQueryData(queryFile);

    LSH * MyLsh = new LSH(K,L);
    MyLsh->Train();
    
    int limit = 5;

    // Check if the file exists and delete it if it does
    if (std::ifstream(outputfileName)) {
        std::remove(outputfileName);
    }

    // // Check if the file is opened successfully
    // if (!outputFile.is_open()) {
    //     std::cerr << "Error: Could not open the file." << std::endl;
    //     return 1;
    // }

    
    for (int i = 0 ; i < limit ; i++){
        outputFile.open(outputfileName, std::ios::app);
        outputFile << "Query : "<<i<<std::endl;
       // DisplayQueryData(i);
        outputFile << "Result\n";
        outputFile.close();

        MyLsh->KNN(N,GetQueryRepresentation(i));
        //MyLsh->AccurateKNN(K,GetQueryRepresentation(i));
        //for (int j = 0 ; j < K ; j++)
           // DisplayTrainData(Result[j]);
        //DisplayTrainData(MyLsh->NearestNeighbour(GetQueryRepresentation(i)));
        vector <int> Range = MyLsh->RangeSearch(1600,GetQueryRepresentation(i));
        outputFile.open(outputfileName, std::ios::app);
        outputFile << "R-near neighbors:\n";
        outputFile.close();
        for (int j = 0 ; j < (int)Range.size();j++)
        {
            outputFile.open(outputfileName, std::ios::app);
            outputFile << Range[j] << "\n";
            outputFile.close();
        }
    }

    outputFile.open(outputfileName, std::ios::app);
    outputFile << Euclidean2(GetQueryRepresentation(0),GetRepresenation(8476),784);
    outputFile.close();

    return 0;
}




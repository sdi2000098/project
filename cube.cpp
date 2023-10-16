#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "RandomProjection.h"
#include <fstream> // For file stream operations

const char * outputfileName;
std::ofstream outputFile;


int main(int argc, char* argv[]) {
    /*
    std::string inputFile, queryFile;

    int K = 4;
    int N = 5;


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
        } else if (arg == "-o" && i + 1 < argc) {
            outputfileName = argv[i + 1];
            i++;
        } else if (arg == "-N" && i + 1 < argc) {
            N = std::stoi(argv[i + 1]);
            i++;
        }
    }

    */

    ReadTrainData("dataset.dat");
    ReadQueryData("query.dat");

    RandomProjection * MyCube = new RandomProjection(100,5,45);
    MyCube->Train();
    
    int limit = 5;

    // Check if the file exists and delete it if it does
    /*
    
    if (std::ifstream(outputfileName)) {
        std::remove(outputfileName);
    }

    */

    // // Check if the file is opened successfully
    // if (!outputFile.is_open()) {
    //     std::cerr << "Error: Could not open the file." << std::endl;
    //     return 1;
    // }

    
    for (int i = 0 ; i < limit ; i++){
        //outputFile.open(outputfileName, std::ios::app);
        //outputFile << "Query : "<<i<<std::endl;
       // DisplayQueryData(i);
        //outputFile << "Result\n";
        //outputFile.close();
        MyCube->KNN(10,GetQueryRepresentation(i));
        //MyLsh->AccurateKNN(K,GetQueryRepresentation(i));
        //for (int j = 0 ; j < K ; j++)
           // DisplayTrainData(Result[j]);
        //DisplayTrainData(MyLsh->NearestNeighbour(GetQueryRepresentation(i)));

    }


    return 0;
}
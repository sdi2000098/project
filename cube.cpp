#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "RandomProjection.h"
#include <fstream> // For file stream operations
#include <vector>
#define ERROR -1

//Same code with main.cpp but now we use hypercube code

int main(int argc, char* argv[]) {
    const char * outputfileName;
    std::string inputFile, queryFile,answer;
    std::ofstream outputFile;

    int K = 14;          
    int N = 1;
    int M = 10;         //M needs to be at least 2 times N
    int probes = 2;
    double R = 10000;

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
        } else if (arg == "-probes" && i + 1 < argc) {
            probes = std::stoi(argv[i + 1]);
            i++;
        } else if (arg == "-R" && i + 1 < argc) {
            R = std::stod(argv[i + 1]);
            i++;
        
        } else if (arg == "-M" && i + 1 < argc) {
            M = std::stod(argv[i + 1]);
            i++;
        }
        else{
            cout << "Unexpected argument in command line\n";
            return ERROR;
        }
    }

    if (K<= 0 || M <= 0 || N <= 0 || R <=0 || probes < 0){
        cout << "K,L,N,R need to be positive integers\n";
        return ERROR;
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

    do{
    
        if ( ReadQueryData(queryFile) == ERROR)
            return ERROR;

        RandomProjection * MyCube = new RandomProjection(K,M,probes);
        
        MyCube->Train();
        int limit = GetQueryNumber();
        limit = 5;
        // Check if the file exists and delete it if it does
        
        if (std::ifstream(outputfileName)) {
            std::remove(outputfileName);
        }
        outputFile.open(outputfileName, std::ios::app);
        // // Check if the file is opened successfully
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open the output file." << std::endl;
            return ERROR;
        }

        vector <double> KNNResult, AcuurateKNNReult;    
        clock_t start, end;
        double KNNTIme = 0,AccurateKNNTime = 0;
        double MAF[limit];
        for (int i = 0 ; i < limit ; i++){
            outputFile << "Query : "<<i<<std::endl;

            start = clock();
            KNNResult = MyCube->KNN(N,GetQueryRepresentation(i));
            end = clock();
            KNNTIme = double(end - start) / double(CLOCKS_PER_SEC);

            start = clock();
            
            AcuurateKNNReult = MyCube->AccurateKNN(N,GetQueryRepresentation(i));
            end = clock();
            AccurateKNNTime = double(end - start) / double(CLOCKS_PER_SEC);

            for (int j = 0 ; j < 2*N ; j+=2 ){
                if ( j == 0 && (int)KNNResult.size() != 0)
                    MAF[i]= KNNResult[j]/AcuurateKNNReult[j];
                if ( j < (int)KNNResult.size()){
                    outputFile << "Nearest neighbor-"<<j/2 +1<< ": " << KNNResult[j+1] << "\n";
                    outputFile << "distanceHypercube: " << KNNResult[j] <<"\n";
                }
                else
                    outputFile << "Could not find Nearest neighbor " << j/2 << " using aproximate KNN\n";
                if (j <(int) AcuurateKNNReult.size())
                    outputFile << "distanceTrue: " << AcuurateKNNReult[j] << "\n";
                else 
                    outputFile << "Could not find Nearest neighbor " << j/2 << " using exhaustive KNN\n";

            }

            outputFile << "tHypercube: " << KNNTIme << "\ntTrue: " << AccurateKNNTime <<"\n";
            vector <int> Range = MyCube->RangeSearch(R,GetQueryRepresentation(i));
            outputFile << "R-near neighbors:\n";
            for (int j = 0 ; j < (int)Range.size();j++)
                outputFile << Range[j] << "\n";

            
        }
        double maxmaf=MAF[0];

        for (int i=1;i<limit;i++)
            if (MAF[i] > maxmaf)
                maxmaf = MAF[i];
        outputFile << "MAF : " << maxmaf << "\n";
        outputFile.close();
        delete MyCube;
        std::cout<<"Terminate program? (y/n)\n";
        std::cin>>answer;
        if (answer=="y")
            break;
        else
        {
            std::cout<<"Give queryfile\n";

            std::cin >> queryFile;
        }
    } while(answer=="n");
    DeleteQueries();
    DeleteTrain();
    return 0;
}
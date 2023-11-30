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
using namespace std;
int main (int argc, char* argv[]){
    const char * outputfileName = NULL;
    ofstream outputFile;
    string inputFile , queryFile,answer;

    int L = 20,N = 1,k=50, E=30,R = 1, m = 0;
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
    
    if ( ReadTrainData(inputFile) == ERROR) //read dataset
        return ERROR;
    int limit = GetTrainNumber(),Navigating;
    
    do{
        if ( ReadQueryData(queryFile) == ERROR) //read query file
            return ERROR;
        
        GraphGNN * graph1;
        GraphMRNG * graph2;

        if (m == 1){
            graph1 = createGraphGNN(limit,k);  //for gnn algorithm
            CreateGnn(graph1,k); //initialization and creation of graph
        }
        else{
            graph2 = createGraphMRNG(limit); //for mrng algorithm
            CreateMrng(graph2); //initialization and creation of graph
            Navigating = FindNavigating();
        }
        if (std::ifstream(outputfileName)) {
            std::remove(outputfileName);
        }
        outputFile.open(outputfileName, std::ios::app);
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open the output file." << std::endl;
            return ERROR;
        }
        //int limit2 = GetQueryNumber();
        int limit2 = 5;
        clock_t start, end;
        if (m == 1)
            outputFile << "GNN Results\n";
        else
            outputFile << "MRNG Results\n";
        double GraphSearchTime =0 ,AccurateTime =0;
        double MAF[limit2];
        
        for (int i = 0 ; i < limit2;i++){
            outputFile << "Query : " << i << "\n";
            vector<double *> currentResult;
            if (m == 1){
                start = clock();
                currentResult = GNNS(graph1, GetQueryRepresentation(i), R, 50, E,N);
                end = clock();
            }
            else{
                start = clock();
                currentResult = GenericGraphSearch(graph2,Navigating,GetQueryRepresentation(i),L,N);
                end = clock();
            }
            
            GraphSearchTime += double(end - start) / double(CLOCKS_PER_SEC);
            start = clock();
            double * TrueNeighbor = FindTrue(GetQueryRepresentation(i),N);
            end = clock();
            AccurateTime += double(end - start) / double(CLOCKS_PER_SEC);
            
            for (int j = 0 ; j < (int)currentResult.size();j++){
                
                if ( j == 0 )
                    MAF[i]= currentResult[j][DISTANCE]/TrueNeighbor[j]; //store in a table all mafs in order to take the max later
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
            outputFile << "------------------------------------------------\n";
            
            if (m == 2)
                for (int j = 0 ; j < (int)currentResult.size();j++)
                    delete [] currentResult[j];
            
            delete []TrueNeighbor;
        }

        double maxmaf=MAF[0];

        for (int i=1;i<limit2;i++)
            if (MAF[i] > maxmaf)
                maxmaf = MAF[i];

        outputFile << "tAverageApproximate : " << GraphSearchTime/limit2 <<"\n";
        outputFile << "tAverageTrue : " << AccurateTime/limit2 << "\n";
        outputFile << "MAF : " << maxmaf << "\n";
        outputFile.close();
        
        cout<<"Terminate program? (y/n)\n";
        cin>>answer;
        while (answer != "n" && answer != "y")
        {
            cout << "Wrong input, please insert y to terminate or n to continue\n";
            cin >> answer;
        }
        if (answer=="y"){          //free variables
            if (m == 1)
                DeleteGraph(graph1);
            else
                DeleteGraph(graph2);
            break;
        }
        else
        {
            cout<<"Give queryfile\n";
            cin >> queryFile;
        }
        if (m == 1)
            DeleteGraph(graph1);
        else
            DeleteGraph(graph2);
        outputFile.close();
        DeleteQueries();
    }while(answer=="n");
    
    DeleteQueries();
    DeleteTrain();
    
}
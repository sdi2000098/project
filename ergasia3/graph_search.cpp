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
#define GREEDY_STEPS 50 //define t
using namespace std;
int main (int argc, char* argv[]){
    const char * outputfileName = NULL;
    ofstream outputFile;
    string inputFile , queryFile,answer;

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
    int limit = GetTrainNumber(),Navigating = 0; //limit = number of images
    double MeanApproximationFactor=0;
    //do{
        if ( ReadQueryData(queryFile) == ERROR) //read query file
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
        if (std::ifstream(outputfileName)) {
            std::remove(outputfileName);
        }
        outputFile.open(outputfileName, std::ios::app);
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open the output file." << std::endl;
            return ERROR;
        }
        int limit2 = GetQueryNumber(); //limit2 number of queries
        limit2 = 20;
        clock_t start, end;
        if (m == 1)
            outputFile << "GNN Results\n";
        else
            outputFile << "MRNG Results\n";
        double GraphSearchTime =0 ,AccurateTime =0;
        MeanApproximationFactor = 0;
        for (int i = 0 ; i < limit2;i++){ //for all queries
            
            outputFile << "Query : " << i << "\n";
            vector<double *> currentResult;
            //Current result is a vector of double arrays, each array has 2 values : position in dataset, distance of point from query
            
            if (m == 1){
                start = clock();
                currentResult = GNNS(graph1, GetQueryRepresentation(i), R, GREEDY_STEPS, E,N); //gnn algorithm
                end = clock();
            }
            else{
                start = clock();
                currentResult = GenericGraphSearch(graph2,Navigating,GetQueryRepresentation(i),L,N); //mrng algorithm
                end = clock();
            }
            
            GraphSearchTime += double(end - start) / double(CLOCKS_PER_SEC); //approximate time
            start = clock();
            double * TrueNeighbor = FindTrue(GetQueryRepresentation(i),N); //find true neighbors
            end = clock();
            AccurateTime += double(end - start) / double(CLOCKS_PER_SEC); //accurate time
            
            for (int j = 0 ; j < (int)currentResult.size();j++){
                
                MeanApproximationFactor += currentResult[j][DISTANCE]/TrueNeighbor[j];
                if (currentResult[j][POSITION] == ERROR){
                    outputFile << "Could not fine approximate nearest neighbor" << j  <<"\n";
                    continue;
                }
                if (TrueNeighbor[j] == ERROR){
                    outputFile << "Could not fine accuate nearest neighbor" << j  <<"\n";
                    continue;
                }
                
                outputFile << "Nearest neighbor-" << j+1 << " : " << currentResult[j][POSITION] << "\n"; //print in file
                
                outputFile << "distanceApproximate : " << currentResult[j][DISTANCE] <<"\n";
                
                outputFile << "distanceTrue : " << TrueNeighbor[j] <<"\n";
            }
            
            outputFile << "------------------------------------------------\n";
            
            for (int j = 0 ; j < (int)currentResult.size();j++) //free this array
                delete [] currentResult[j];
            
            delete []TrueNeighbor;
            
        }


        outputFile << "tAverageApproximate : " << GraphSearchTime/limit2 <<"\n"; //print time and maf in file
        outputFile << "tAverageTrue : " << AccurateTime/limit2 << "\n";
        MeanApproximationFactor = MeanApproximationFactor/(limit2*N);
        outputFile << "Mean Approximation Factor: " << MeanApproximationFactor << endl;
        outputFile.close();
        
        //cout<<"Terminate program? (y/n)\n";  //y for terminate / n for no and give a query file
        //cin>>answer;
        /*while (answer != "n" && answer != "y") 
        {
            cout << "Wrong input, please insert y to terminate or n to continue\n";
            cin >> answer;
        }
        if (answer=="y"){          //free variables
            if (m == 1)
                DeleteGraph(graph1); //delete graphs
            else
                DeleteGraph(graph2);
            break;
        }
        else
        {
            cout<<"Give queryfile\n";
            cin >> queryFile;
        }*/
        if (m == 1)
            DeleteGraph(graph1);
        else
            DeleteGraph(graph2);
        outputFile.close();
        DeleteQueries();
    //}while(answer=="n"); //while answer is n run again 
    
    DeleteQueries(); //free queries data
    DeleteTrain(); //free images data
    std::cout << "Return Value: " << MeanApproximationFactor << std::endl;
    return (int)MeanApproximationFactor;
}
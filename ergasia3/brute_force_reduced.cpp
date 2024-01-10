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
    string inputFile , queryFile, inputFileReduced, queryFileReduced,outputfileName;
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
        } else if (arg == "-dr" && i + 1 < argc) {      //Add two more arguments for reduced files
            inputFileReduced = argv[i + 1];
            i++;
        } else if (arg == "-qr" && i + 1 < argc) {
            queryFileReduced = argv[i + 1];
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
    if (inputFileReduced.empty()){         //Input file not initialized by command line
        cout << "Please insert path to reduced dataset :\n";
        cin >> inputFileReduced;
    }
    if (queryFileReduced.empty()){         //Query file not initialized by command line
        cout << "Please insert path to reduced query file :\n";
        cin >> queryFileReduced;
    }
    if (inputFile.empty() || queryFile.empty() || outputfileName.empty() || inputFileReduced.empty() || queryFileReduced.empty()){
        cout << "One or more files not initialized\n";
        return ERROR;
    }
    
    if (ReadTrainData(inputFileReduced) == ERROR)   //Read reduced files
        return ERROR;
    if (ReadQueryData(queryFileReduced) == ERROR)
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
    vector <int> indexes[limit];   //Vector that contains the indexes of the K nearest neighbors for each query
    //Will be use to hold indexes to return to original space
    double minDistance;
    int minIndex;
    //Start measuring time
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0;i<limit;i++){    //Find K nearest neighbors for each query in reduced space
        uint8_t * query = GetQueryRepresentation(i);
        SetAllUnchecked();      //For each query we set all train images as unchecked
        for (int j = 0;j<K;j++){    //Find K nearest neighbors
            minDistance = 100000000;
            for (int k = 0;k<GetTrainNumber();k++){   //For each train image
                
                uint8_t * train = GetRepresenation(k);  //Get representation
                double distance = Euclidean(query,train,GetDimension());    //Calculate distance
                if (distance < minDistance && GetChecked(k) == false){    //If distance is smaller than minDistance and image is unchecked
                    minDistance = distance;
                    minIndex = k;
                }
            }
            SetChecked(minIndex);   //Set image as checked
            indexes[i].push_back(minIndex); //Insert index to vector
        }   
    }
    auto stop = chrono::high_resolution_clock::now();
    if (ReadTrainData(inputFile) == ERROR)  //Read original files
        return NULL;
    if (ReadQueryData(queryFile) == ERROR)
        return NULL;
    double * Distances;
    
    for (int i = 0;i<limit;i++){    //For each query calculate true distances and print them
        outputFile << "Query : "<<i<<std::endl;
        Distances = GetTrueDistances(indexes[i],GetQueryRepresentation(i));     // Get back to original space and calculate true distances
        for (int j = 0;j<K;j++){
            outputFile << "Nearest neighbor-" << j+1 <<": "<<indexes[i][j]<<endl;
            outputFile << "distance: "<<Distances[j]<<endl;
        }
        delete [] Distances;
    }
    chrono::duration<double> duration = stop - start;
    outputFile << "Average time : " << duration.count()/(limit*K) << " seconds" << endl;
    double MeanApproximationFactor = 0;
    for (int i = 0 ; i < limit ; i ++){ 
        //We need to calclualte k true neighbors for each query in original space in order to calculate mean approximation factor
        uint8_t * query = GetQueryRepresentation(i);
        SetAllUnchecked();
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
            MeanApproximationFactor += Euclidean(GetRepresenation(indexes[i][j]),query,GetDimension()) /minDistance;
            //Calculate approximation factor for each neighbor and add it to the sum
        }
        
    }
    MeanApproximationFactor = MeanApproximationFactor/(limit*K);
    outputFile << "Mean Approximation Factor: " << MeanApproximationFactor << endl;
    outputFile.close();
    DeleteQueries();
    DeleteTrain();
    std::cout << "Return Value: " << MeanApproximationFactor << std::endl;
    //This stdout output is used by the python script to get the mean approximation factor
    return (int)MeanApproximationFactor;
}
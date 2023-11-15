#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "lsh.h"
#include <fstream> // For file stream operations
#define ERROR -1



int main(int argc, char* argv[]) {
    const char * outputfileName;
    std::ofstream outputFile;
    std::string inputFile , queryFile, answer;


    //Initialize default values
    int K = 4;
    int L = 5;
    

    int k=5;
    int E=30;
    double R = 1;
    int N=1;



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
        } else if (arg == "-E" && i + 1 < argc) {
            E = std::stoi(argv[i + 1]);
            i++;
        } else if (arg == "-R" && i + 1 < argc) {
            R = std::stoi(argv[i + 1]);
            i++;
        } else if (arg == "-N" && i + 1 < argc) {
            N = std::stoi(argv[i + 1]);
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
    if (k<= 0 || E <= 0 || N <= 0 || R <=0){
        cout << "K,L,N,R need to be positive integers\n";
        return ERROR;
    }
    
    if (inputFile.empty()){         //Input file not initialized by command line
        cout << "Please insert path to dataset :\n";
        cin >> inputFile;
    }

    if ( ReadTrainData(inputFile) == ERROR)
        return ERROR;

    // do{
        LSH * MyLsh = new LSH(K,L);
        MyLsh->Train();

        int limit = GetTrainNumber();

        vector <double> KNNResult; 

        for (int i = 0 ; i < limit ; i++){
            cout << "Query : "<<i<<std::endl;

            KNNResult = MyLsh->KNN(k,GetRepresenation(i));

            for (int j = 0 ; j < 2*k ; j+=2 ){
                
                if ( j < (int)KNNResult.size() && KNNResult[j+1] != -1 ){
                    cout << "Nearest neighbor-"<<j/2 +1<< ": " << KNNResult[j+1] << "\n";
                    cout << "distanceLSH: " << KNNResult[j] <<"\n";
                }
                else
                    cout << "Could not find Nearest neighbor " << j/2 << " using aproximate KNN\n";
            }
        }


    //     std::cout<<"Terminate program? (y/n)\n";
    //     std::cin>>answer;
    //     if (answer=="y")
    //         exit(0);
    //     else
    //     {
    //         std::cout<<"Give queryfile\n";

    //         std::cin >> queryFile;
    //     }
    // } while(answer=="n");

    return 0;
}


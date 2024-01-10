#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "ReadTrainData.h"
#include "kmeans.h"
#include <cstring>
#define ERROR -1

// Function to read arguments from a file and set defaults
std::map<std::string, int> readArgumentsFromFile(const std::string& filename) {
    // Define default values
    std::map<std::string, int> defaults;
    defaults["number_of_vector_hash_tables"] = 8;
    defaults["number_of_vector_hash_functions"] = 2;
    defaults["max_number_M_hypercube"] = 96;
    defaults["number_of_hypercube_dimensions"] = 6;
    defaults["number_of_probes"] = 2;

    // Open the file
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening configuration file." << std::endl;
        exit(ERROR);
        return defaults;
    }

    // Read the file line by line
    std::string line;
    while (std::getline(inputFile, line)) {
        size_t pos = line.find(':');
            std::string str = line.substr(0, pos);  //extract a substring from position 0 to poisition pos of line.
            std::string value = line.substr(pos + 1); //here is the value
            int intValue = std::stoi(value);
            defaults[str] = intValue;  
  
    }

    // Close the file
    inputFile.close();

    return defaults;
}

int main (int argc, char * argv[]){

    bool flag = false;

    std::string inputFile , queryFile,configFile;
    const char * outputfileName;
    char * Method = NULL;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        //Check for flags and change values if needed
        if (arg == "-i" && i + 1 < argc) {
            inputFile = argv[i + 1];
            i++;
        }
        else if (arg == "-o" && i + 1 < argc) {
            outputfileName = argv[i + 1];
            i++;
        }
        else if (arg == "-c" && i + 1 < argc) {
            configFile = argv[i + 1];
            i++;
        }
        else if (arg == "-complete" ) {
            flag = true;
        }
        else if (arg == "-m" && i + 1 < argc) {
            Method = (char*)malloc(strlen(argv[i+1])*sizeof(char));
            strcpy(Method,argv[i+1]);
            i++;
        }
        else{
            cout << "Unexpected argument in command line\n";
            return ERROR;
        }
    }
    
    // Provide the filename as an argument to the function
    std::map<std::string, int> arguments = readArgumentsFromFile(configFile);

    // Initialize variables with values from the arguments
    int K = arguments["number_of_clusters"];
    int L = arguments["number_of_vector_hash_tables"];
    int KLSH = arguments["number_of_vector_hash_functions"];
    int M = arguments["max_number_M_hypercube"];
    int Kcube = arguments["number_of_hypercube_dimensions"];
    int probes = arguments["number_of_probes"];
    if (K<= 0 || M <= 0 || L <= 0 || KLSH <=0 || probes < 0 || Kcube <= 0){
        cout << "K,L,N,R need to be positive integers\n";
        return ERROR;
    }

    if (inputFile.empty()){         //Input file not initialized by command line
        cout << "Please insert path to dataset :\n";
        cin >> inputFile;
    }
    if ( ReadTrainData(inputFile) == ERROR)
        return ERROR;
    if (std::ifstream(outputfileName)) {
        std::remove(outputfileName);
    }
    
    if (Method == NULL)
    {
        std::cerr << "Please provide a method" << std::endl;
        return ERROR;
    }
    KMeans * MyCluster = new KMeans(K,Method,KLSH,L,Kcube,M,probes,outputfileName,flag);
    free(Method);
    delete MyCluster;
    DeleteTrain();
}
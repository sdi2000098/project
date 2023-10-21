#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "ReadTrainData.h"
#include "kmeans.h"
#include <cstring>


// Function to read arguments from a file and set defaults
std::map<std::string, int> readArgumentsFromFile(const std::string& filename) {
    // Define default values
    std::map<std::string, int> defaults;
    defaults["number_of_vector_hash_tables"] = 3;
    defaults["number_of_vector_hash_functions"] = 4;
    defaults["max_number_M_hypercube"] = 10;
    defaults["number_of_hypercube_dimensions"] = 3;
    defaults["number_of_probes"] = 2;

    // Open the file
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file." << std::endl;
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

int main (void){

    // Provide the filename as an argument to the function
    std::map<std::string, int> arguments = readArgumentsFromFile("cluster.conf");

    // Initialize variables with values from the arguments
    int K = arguments["number_of_clusters"];
    int L = arguments["number_of_vector_hash_tables"];
    int KLSH = arguments["number_of_vector_hash_functions"];
    int M = arguments["max_number_M_hypercube"];
    int Kcube = arguments["number_of_hypercube_dimensions"];
    int probes = arguments["number_of_probes"];

    ReadTrainData("dataset.dat");
    char * x = new char[sizeof("Classic")];
    strcpy(x,"Classic");
    KMeans * MyCluster = new KMeans(K,x,KLSH,L,Kcube,M,probes);
}
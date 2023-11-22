#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "lsh.h"
#include <fstream> // For file stream operations
#include <omp.h>
#include <random>
#include <algorithm>
#include "hFunc.h"
#define ERROR -1
#define KLSH 5
#define LLSH 5
#define DIMENSION 784
#define CANDITATES 50
struct Graph {
    int vertices,k;
    double *** adjList; // Array of adjacency lists
};


int main (void){
    const char * outputfileName;
    ofstream outputFile;
    string inputFile , queryFile;

    int L = 20,N = 5,j;
    int limit = GetTrainNumber();
    vector <double> Rp; 
    vector <int> Lp;
    LSH * MyLsh = new LSH(KLSH,LLSH);

    for (int p = 0 ; p < limit ; p++){
        Rp = MyLsh->KNN(CANDITATES,GetRepresenation(p),p);
        Lp.push_back((int)Rp[1]);
        for (j = 2; j < 2*CANDITATES;j+=2)
            if (Rp[j]==Rp[0])
                Lp.push_back(Rp[j+1]);
        
        for (int q = 1 ; q < Rp.size();q+=2){
            bool flag = true;
            for (int r = 0 ; r <Lp.size();r++){
                double pq = Euclidean(GetRepresenation(Rp[q]),GetRepresenation(p),DIMENSION);
                double pr = Euclidean(GetRepresenation(p),GetRepresenation(Lp[r]),DIMENSION);
                double qr = Euclidean(GetRepresenation(Rp[q]),GetRepresenation(Lp[r]),DIMENSION);
                if (pq > pr && pq >qr){
                    flag = false;
                    break;
                }
            }
            if (flag)
                Lp.push_back(Rp[q]);
        }
    }
}
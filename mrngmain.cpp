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
#define POSITION 0
#define DISTANCE 1
struct Graph {
    int vertices;
    vector <double *> * adjList; // Array of adjacency lists
};
struct Graph* createGraph(int vertices) {

    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->vertices = vertices;

    graph->adjList = new vector <double *> [vertices];
    return graph;
}
void addEdge(struct Graph* graph, int src, double * NewNeighbor) {
    graph->adjList[src].push_back(NewNeighbor);
}
double * GetEdge(struct Graph* graph, int src,int Position){
    return graph->adjList[src][Position];
}
int EdgesNumber(struct Graph* graph, int src){
    return (int)graph->adjList[src].size();
}
void PrintGraph(Graph * graph){
    for (int i = 0 ; i < graph->vertices;i++){
        cout << "Adjacency list for " << i << " : ";
        for (int j = 0; j < (int)graph->adjList[i].size() ; j++)
            printf("%d (%f), ",(int)graph->adjList[i][j][POSITION],graph->adjList[i][j][DISTANCE]);
    }
}

double * FindTrue(uint8_t * Query,int N){
    double * ToReturn = new double [N];
    for (int n = 0 ; n < N; n++){

        double minDistance = numeric_limits<double>::max();
        for (int i = 0 ; i < GetTrainNumber() ;i ++){
            double e = Euclidean(GetRepresenation(i),Query,DIMENSION);
            if ( e < minDistance){
                bool flag = false;
                for (int j = 0 ; j < n ; j++){
                    if (ToReturn[j] == e){
                        flag = true;
                        break;
                    }
                }
                if (flag)
                    continue;
                minDistance = e ;
            }
        }
        ToReturn[n] = minDistance;

    }
    return ToReturn;
}

bool compareDistances(const double* a, const double* b) {
    return a[DISTANCE] < b[DISTANCE];
}
#define CHECKED 2
vector <double *> GenericGraphSearch(Graph * graph,int p, uint8_t * q,int L,int N){
    vector <double *> R;
    double * ToPush = new double[3];
    ToPush[POSITION] = p;
    ToPush[DISTANCE] = Euclidean(GetRepresenation(p),q,DIMENSION);
    ToPush[CHECKED] = 0;
    R.push_back(ToPush);
    for (int i = 1 ; i < L ; i++){
        for (p = 0;p<(int)R.size();p++)
            if (R[p][CHECKED] == 0)
                break;
        R[p][CHECKED] = 1;
        //R[p][POSITION] position in dataser
        for (int j = 0 ; j < EdgesNumber(graph,R[p][POSITION]);j++){
            bool flag = true;
            double * N = GetEdge(graph,R[p][POSITION],j);
            for (int k = 0 ; k < (int)R.size() ; k++){
                if (R[k][POSITION] == N[POSITION]){
                    flag = false;
                    break;
                }
            }
            if (flag){
                ToPush = new double[3];
                ToPush[POSITION] = N[POSITION];
                ToPush[DISTANCE] = Euclidean(GetRepresenation(N[POSITION]),q,DIMENSION);
                ToPush[CHECKED] = 0;
                R.push_back(ToPush);
            }
            sort(R.begin(),R.end(),compareDistances);
        }
    }
    vector <double *> result(R.begin(),R.begin()+N);
    return result;
}


int main (int argc, char* argv[]){
    const char * outputfileName;
    ofstream outputFile;
    string inputFile , queryFile;

    int L = 20,N = 5,j;

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
        else{
            cout << "Unexpected argument in command line\n";
            return ERROR;
        }
    }
    if (N <= 0 || L <=0){
        cout << "L,N need to be positive integers\n";
        return ERROR;
    }
    
    if (inputFile.empty()){         //Input file not initialized by command line
        cout << "Please insert path to dataset :\n";
        cin >> inputFile;
    }
    if (ifstream(outputfileName)) {
            remove(outputfileName);
        }
    outputFile.open(outputfileName, ios::app);
    if (!outputFile.is_open()) {
        cerr << "Error: Could not open the output file." << endl;
        return ERROR;
    }
    if ( ReadTrainData(inputFile) == ERROR)
        return ERROR;



    int limit = GetTrainNumber();

    Graph * graph = createGraph(limit);
    
    LSH * MyLsh = new LSH(KLSH,LLSH);
    MyLsh->Train();

    double * ToPush;
    
    for (int p = 0 ; p < limit ; p++){
        
        vector <double> Rp; 
        
        Rp = MyLsh->KNN(CANDITATES,GetRepresenation(p),p);
        
        ToPush = new double[2];
        ToPush[POSITION] = Rp[1];
        ToPush[DISTANCE] = Rp[0];
        
        addEdge(graph,p,ToPush);
        
        for (j = 2; j < 2*CANDITATES;j+=2){
            if (Rp[j]==Rp[0]){
                ToPush = new double[2];
                ToPush[POSITION] = Rp[j+1];
                ToPush[DISTANCE] = Rp[j];
                addEdge(graph,p,ToPush);
            }
        }
        
        for (int q = 3 ; q <(int) Rp.size();q+=2){
            
            bool flag = true;
            for (int r = 0 ; r <EdgesNumber(graph,p);r++){
                if ((int)Rp[q] == ERROR){
                    flag = false;
                    break;
                }
                double pq = Euclidean(GetRepresenation((int)Rp[q]),GetRepresenation(p),DIMENSION);
                double * Neighbor = GetEdge(graph,p,r);
                double pr = Neighbor[DISTANCE];
                double qr = Euclidean(GetRepresenation((int)Rp[q]),GetRepresenation(Neighbor[POSITION]),DIMENSION);
                if (pq > pr && pq >qr){
                    flag = false;
                    break;
                }
            }
            
            if (flag){
                ToPush = new double[2];
                ToPush[POSITION] = Rp[q];
                ToPush[DISTANCE] = Rp[q-1];
                addEdge(graph,p,ToPush);
            }
        }
    }
    double * Centroid=new double[DIMENSION];
    for (int i = 0 ; i<DIMENSION;i++)
        Centroid[i] = 0;
    for (int i = 0 ;i<limit;i++){
        uint8_t * temp = GetRepresenation(i);
        for (int j = 0 ; j < DIMENSION;j++)
            Centroid[j]+=temp[j];
    }
    uint8_t * BytesCentroid = new uint8_t[DIMENSION];
    for (int i = 0 ; i < DIMENSION;i++){
        BytesCentroid[i]=(uint8_t)Centroid[i];
    }
    int Navigating;
    double Min =  numeric_limits<double>::max();
    for (int i = 0 ; i<limit;i++){
        double e = Euclidean(BytesCentroid,GetRepresenation(i),DIMENSION);
        if (e < Min){
            Min = e;
            Navigating = i;
        }
    }
    if ( ReadQueryData(queryFile) == ERROR)
        return ERROR;
    //int limit2 = GetQueryNumber();
    int limit2 = 5;
    clock_t start, end;
    double GenericSearchTime ,AccurateTime ;

    for (int i = 0 ; i < limit2;i++){
        start = clock();
        vector<double *> currentResult = GenericGraphSearch(graph,Navigating,GetQueryRepresentation(i),L,N);
        end = clock();
        GenericSearchTime = double(end - start) / double(CLOCKS_PER_SEC);
        
        start = clock();
        double * TrueNeighbor = FindTrue(GetQueryRepresentation(i),N);
        end = clock();
        AccurateTime = double(end - start) / double(CLOCKS_PER_SEC);
        for (int j = 0 ; j < (int)currentResult.size();j++){
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
        
        outputFile << "tAverageApproximate : " << GenericSearchTime <<"\n";
        outputFile << "tAverageTrue : " << AccurateTime << "\n\n";
        outputFile << "------------------------------------------------\n";
    }
}
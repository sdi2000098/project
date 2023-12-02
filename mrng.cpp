#include "mrng.h"

struct GraphMRNG {
    int vertices;
    vector <double *> * adjList; 
};
struct GraphMRNG* createGraphMRNG(int vertices) {

    struct GraphMRNG* graph = (struct GraphMRNG*)malloc(sizeof(struct GraphMRNG));
    graph->vertices = vertices;

    graph->adjList = new vector <double *> [vertices];
    return graph;
}
void addEdge(struct GraphMRNG* graph, int src, double * NewNeighbor) {
    graph->adjList[src].push_back(NewNeighbor);
}
double * GetEdge(struct GraphMRNG* graph, int src,int Position){
    return graph->adjList[src][Position];
}
int EdgesNumber(struct GraphMRNG* graph, int src){
    return (int)graph->adjList[src].size();
}
void PrintGraph(GraphMRNG * graph){
    for (int i = 0 ; i < graph->vertices;i++){
        cout << "Adjacency list for " << i << " : ";
        for (int j = 0; j < (int)graph->adjList[i].size() ; j++)
            printf("%d (%f), ",(int)graph->adjList[i][j][POSITION],graph->adjList[i][j][DISTANCE]);
    }
}

bool compareDistances2(const double* a, const double* b) {
    return a[DISTANCE] < b[DISTANCE];
}

vector <double *> GenericGraphSearch(GraphMRNG * graph,int p, uint8_t * r,int L,int N){
    
    vector <double *> R;
    double * ToPush = new double[3];
    ToPush[POSITION] = p;
    ToPush[DISTANCE] = Euclidean(GetRepresenation(p),r,DIMENSION);
    ToPush[CHECKED] = 0;
    R.push_back(ToPush);
    
    for (int i = 1 ; i < L ; i++){
        
        for (p = 0;p<(int)R.size();p++)
            if (R[p][CHECKED] == 0)
                break;
        if (p == (int)R.size())
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
                ToPush[DISTANCE] = Euclidean(GetRepresenation(N[POSITION]),r,DIMENSION);
                ToPush[CHECKED] = 0;
                R.push_back(ToPush);
            }
            sort(R.begin(),R.end(),compareDistances2);
        }
        
    }
    
    for (size_t i = N; i < R.size(); ++i) {
        delete[] R[i];
    }
    
    vector <double *> result(R.begin(),R.begin()+N);
    
    return result;
}


void CreateMrng(GraphMRNG * graph){
    int j;
    int limit = GetTrainNumber();
    LSH * MyLsh = new LSH(KLSH,LLSH);
    MyLsh->Train();
    double * ToPush;
    for (int p = 0 ; p < limit ; p++){  //for every point in dataset
        vector <double> Rp; 
        
        Rp = MyLsh->KNN(CANDITATES,GetRepresenation(p),p);  //get its neighbors from lsh
        
        ToPush = new double[2]; //add the nearest neighbor
        ToPush[POSITION] = Rp[1]; 
        ToPush[DISTANCE] = Rp[0];
        
        addEdge(graph,p,ToPush);    
        int index = 3;
        for (j = 2; j < 2*CANDITATES;j+=2){     // Check if more than one neighbors are at the same distance
            if (Rp[j]==Rp[0]){
                index +=2;
                ToPush = new double[2];
                ToPush[POSITION] = Rp[j+1];
                ToPush[DISTANCE] = Rp[j];
                addEdge(graph,p,ToPush);
            }
        }
        
        for (int r = index ; r <(int) Rp.size();r+=2){  // for r ∈ Rp − Lp 
            
            bool flag = true;
            for (int t = 0 ; t <EdgesNumber(graph,p);t++){      // this is for all t in Lp
                if ((int)Rp[r] == ERROR){
                    flag = false;
                    break;
                }
                double pr=Rp[r-1];      // pr = dist(p, r)
                double * Neighbor = GetEdge(graph,p,t); // pt = dist(p, t)
                double pt = Neighbor[DISTANCE];
                double rt = Euclidean(GetRepresenation((int)Rp[r]),GetRepresenation(Neighbor[POSITION]),DIMENSION);
                if (pr > pt && pr >rt){
                    flag = false;
                    break;
                }
            }
            if (flag){
                ToPush = new double[2];
                ToPush[POSITION] = Rp[r];
                ToPush[DISTANCE] = Rp[r-1];
                addEdge(graph,p,ToPush);
            }
        }
    }
    delete MyLsh;
}

int FindNavigating(void){
    int limit = GetTrainNumber();
    double * Centroid=new double[DIMENSION];
    for (int i = 0 ; i<DIMENSION;i++)
        Centroid[i] = 0;
    for (int i = 0 ;i<limit;i++){
        uint8_t * temp = GetRepresenation(i);
        for (int j = 0 ; j < DIMENSION;j++)
            Centroid[j]+=temp[j]/limit;
    }
    uint8_t * BytesCentroid = new uint8_t[DIMENSION];
    for (int i = 0 ; i < DIMENSION;i++){
        BytesCentroid[i]=(uint8_t)Centroid[i];
    }
    int Navigating = 0;
    double Min =  numeric_limits<double>::max();
    for (int i = 0 ; i<limit;i++){
        double e = Euclidean(BytesCentroid,GetRepresenation(i),DIMENSION);
        if (e < Min){
            Min = e;
            Navigating = i;
        }
    }
    delete []Centroid;
    delete []BytesCentroid;
    return Navigating;
}

void DeleteGraph(GraphMRNG *graph){
    for (int i = 0 ; i < graph->vertices;i++){
        for (int j = 0 ; j < (int)graph->adjList[i].size();j++)
            delete [] graph->adjList[i][j];
    }
    delete [] graph->adjList;
    free(graph);
}
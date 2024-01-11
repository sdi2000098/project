#include "mrng.h"
//Keep in mind that the graph each neighbor is represented by a double array
struct GraphMRNG {
    int vertices;
    vector <double *> * adjList; //Each node in graph has a list of neighbors represented by a double array
    //This double array has 2 values : position in dataset, distance from node to neighbor
};
struct GraphMRNG* createGraphMRNG(int vertices) {
    //Initialize graph, allocate memory for vertices and adjacency list
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

bool compareDistances2(const double* a, const double* b) {
    return a[DISTANCE] < b[DISTANCE];
}

vector <double *> GenericGraphSearch(GraphMRNG * graph,int p, uint8_t * r,int L,int N){
    //This function implements the Generic Graph Search algorithm
    //It returns the N nearest neighbors of r as a vector of double arrays
    //The double array has 3 values : position in dataset, distance point to query, checked
    //Checked is used to check if a node has been checked already, and the main function wont use it
    //P is the index of the navigating node
    //R is the query in bytes
    int DIMENSION = GetDimension();
    vector <double *> R;
    double * ToPush = new double[3];
    ToPush[POSITION] = p;
    ToPush[DISTANCE] = Euclidean(GetRepresenation(p),r,DIMENSION);
    ToPush[CHECKED] = 0;
    R.push_back(ToPush);    //Add navigating node to R
    
    for (int i = 1 ; i < L ; i++){   //for every candidate
        //R is sorted by distance
        
        for (p = 0;p<(int)R.size();p++) //Find first unchecked node
            if (R[p][CHECKED] == 0)
                break;
        if (p == (int)R.size()) //if all nodes are checked break
            break;
        R[p][CHECKED] = 1;  // set checked to 1
        
        for (int j = 0 ; j < EdgesNumber(graph,R[p][POSITION]);j++){    //for every neighbor of p
            bool flag = true;
            double * N = GetEdge(graph,R[p][POSITION],j);
            for (int k = 0 ; k < (int)R.size() ; k++){  //Check if neighbor is already in R
                if (R[k][POSITION] == N[POSITION]){ 
                    flag = false;
                    break;
                }
            }
            if (flag){ //if neighbor is not in R add it
                ToPush = new double[3];
                ToPush[POSITION] = N[POSITION];
                ToPush[DISTANCE] = Euclidean(GetRepresenation(N[POSITION]),r,DIMENSION); //distance from neighbor to query
                ToPush[CHECKED] = 0;
                R.push_back(ToPush);
            }
            sort(R.begin(),R.end(),compareDistances2); //sort R by distance using compareDistances2
        }
        
    }
    
    for (size_t i = N; i < R.size(); ++i) { //delete all nodes after N, since we only need N neighbors
        delete[] R[i];
    }
    
    vector <double *> result(R.begin(),R.begin()+N);
    
    return result;
}


void CreateMrng(GraphMRNG * graph){
    //This function implements the creation of the graph as described in the paper of MRNG
    int j;
    int limit = GetTrainNumber();
    //We use the LSH algorithm to find the neighbors of each node instead of the exhaustive search
    //This is because the exhaustive search is too slow, and the results we get usding LSH are good enough
    //MAF is always less than 1.8
    //In order to use LSH we have defined KLsh and LLsh as 5, cnahgning these values will change the results
    //but the algorithm will still work, theses values are the ones that gave us fast adn good results
    int DIMENSION = GetDimension();
    LSH * MyLsh = new LSH(KLSH,LLSH);
    MyLsh->Train();
    double * ToPush;
    for (int p = 0 ; p < limit ; p++){  //for every point in dataset
        vector <double> Rp; 
        // CANDITATES is the number of neighbors we want to find for each node using LSH
        // We have defined it as 20, changing this value will change the results but the algorithm will still work
        // This value is the one that gave us fast and good results
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
                addEdge(graph,p,ToPush);    //If yes add them to graph
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
                double pt = Neighbor[DISTANCE]; //Keep in mind that int graph we also store the distance from node to neighbor using DISTANCE as index
                double rt = Euclidean(GetRepresenation((int)Rp[r]),GetRepresenation(Neighbor[POSITION]),DIMENSION);
                if (pr > pt && pr >rt){ //Means that is the longest edge in the triangle
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
    //Simple function that finds the navigating node
    int limit = GetTrainNumber();
    int DIMENSION = GetDimension();
    double * Centroid=new double[DIMENSION];
    for (int i = 0 ; i<DIMENSION;i++)
        Centroid[i] = 0;
    //First af all calculate the centroid of the dataset as double array
    for (int i = 0 ;i<limit;i++){
        uint8_t * temp = GetRepresenation(i);
        for (int j = 0 ; j < DIMENSION;j++)
            Centroid[j]+=temp[j]/limit;
    }
    uint8_t * BytesCentroid = new uint8_t[DIMENSION];
    for (int i = 0 ; i < DIMENSION;i++){
        BytesCentroid[i]=(uint8_t)Centroid[i];
        //Normalize the centroid to bytes
    }
    int Navigating = 0;
    double Min =  numeric_limits<double>::max();
    //Using exhaustive search find the nearest neighbor of the centroid
    for (int i = 0 ; i<limit;i++){
        double e = Euclidean(BytesCentroid,GetRepresenation(i),DIMENSION);
        if (e < Min){
            Min = e;
            Navigating = i;
        }
    }
    delete []Centroid;
    delete []BytesCentroid;
    return Navigating;      //return the index of the navigating node
}

void DeleteGraph(GraphMRNG *graph){
    for (int i = 0 ; i < graph->vertices;i++){
        for (int j = 0 ; j < (int)graph->adjList[i].size();j++)
            delete [] graph->adjList[i][j];
    }
    delete [] graph->adjList;
    free(graph);
}
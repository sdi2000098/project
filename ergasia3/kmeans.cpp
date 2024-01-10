#include "kmeans.h"
#include "ReadTrainData.h"
#include <random>
#include <cstring>
#include "hFunc.h"
#include <algorithm>
#include <fstream>
#include "lsh.h"
#include "RandomProjection.h"
#define TABLE_SIZE 5
#define MAXSIZE 1215752202
#define ERROR -1
#define R_THRESHHOLD 2000
std::ofstream outputFile; 

double Euclidean(double * x, uint8_t * y, int D){      //Simple euclidean implementation
//Arguments two vectors and their dimension
    double ToReturn = 0;
    for (int i = 0 ; i < D;i++)
        ToReturn += pow(round(x[i]) - y[i] , 2);
    return sqrt(ToReturn);
}

double Euclidean(double * x, double * y, int D){      //Simple euclidean implementation
//Arguments two vectors and their dimension
    double ToReturn = 0;
    for (int i = 0 ; i < D;i++)
        ToReturn += pow(x[i] - y[i] , 2);
    return sqrt(ToReturn);
}

class Cluster{
    //Class for cluster, each cluster consists of Members that are represented by their position in original dataset
    //There is also the centroid which is an array of doubles
    private : 
        double * Centroid;
        vector <int> Members;
    public : 
        Cluster(int  NewCentroid){
            //In the constructor of the cluster, the input is the first member
            //The centroid will be equal to the new member
            int DIMENSION = GetDimension();
            Centroid = new double [DIMENSION];
            uint8_t * temp = GetRepresenation(NewCentroid);
            for (int i = 0 ; i < DIMENSION ;i ++)
                Centroid[i] = (double) temp[i];
            Members.push_back(NewCentroid);
        }
        ~Cluster(){
            delete [] Centroid;
        };
        void InsertMember(int NewMember){
            int DIMENSION = GetDimension();
            //Insert function with update, used in Lloyds
            //The idea is that NewMean = PrevMean*PrevSize + NewMember / (PrevSize+1)
            for (int i = 0 ; i < DIMENSION; i++)
                Centroid[i] = ( (Centroid[i]*Members.size()) + (double)GetRepresenation(NewMember)[i]) / (Members.size()+1);
            Members.push_back(NewMember);
        }
        double * GetCentroid(void){
            return Centroid;
        }
        void DeleteMember(int ToDelete){
            int DIMENSION = GetDimension();
            Members.erase(std::remove(Members.begin(), Members.end(), ToDelete), Members.end());
            for (int i = 0 ; i < DIMENSION; i++){
                Centroid[i] = ( (Centroid[i]* (Members.size()+1)) - (double)GetRepresenation(ToDelete)[i]) / (double)(Members.size());
                if (Centroid[i] <0)
                    Centroid[i] = 0;
            }
        }
        uint8_t * CentroidAsBytes(void){
            //This function is used in RangeSearch, where the centroid must be query
            //We need a uint8_t representation of the centroid, we round to the closest int
            int DIMENSION = GetDimension();
            uint8_t * ToReturn = new uint8_t[DIMENSION];
            for (int i = 0 ; i < DIMENSION;i++)
                ToReturn[i] = (uint8_t)(round(Centroid[i]));
            return ToReturn;
        }
        int GetMember(int Position){
            return Members[Position];
        }
        int GetSize(void){
            return Members.size();
        }
        void InsertMemberRangeSearch(int NewMember){
            //Insert with no update used in range search
            Members.push_back(NewMember);
        }
        void PrintCentroid(void){
            outputFile << "[ ";
            int DIMENSION = GetDimension();
            for (int i = 0 ; i < DIMENSION ; i++){
                if (i == DIMENSION -1)
                    outputFile << Centroid[i] << " ] ";
                else
                    outputFile << Centroid[i] << ", ";
            }
        }
        void Update(void){
            double accumulator ;
            //Update the center using the current members
            int DIMENSION = GetDimension();
            for (int j = 0 ; j < DIMENSION ; j ++){
                accumulator = 0;
                for (int i =0 ; i < (int)Members.size() ; i ++){
                    accumulator += (double)GetRepresenation(Members[i])[j] / (double)Members.size();
                }
                Centroid[j] = accumulator;
            }
        }
        void UpdateFromReduced(void){
            double accumulator ;
            //Update the center using the current members
            delete [] Centroid;    //Delete old centroid    
            Centroid = new double[GetDimension()];
            int DIMENSION = GetDimension();
            for (int j = 0 ; j < DIMENSION ; j ++){
                accumulator = 0;
                for (int i =0 ; i < (int)Members.size() ; i ++){
                    accumulator += GetRepresenation(Members[i])[j] / (double)Members.size();
                }
                Centroid[j] = accumulator;
            }
        }
        void DisplayFullInfo(void){
            outputFile << "{";
            PrintCentroid();
            for (int i = 0 ; i< (int)Members.size() ; i++ )
                if(i == (int)Members.size() - 1)
                    outputFile << Members[i] <<"}\n";
                else
                    outputFile <<Members[i] << ", ";
        }
};


KMeans :: KMeans(int K_,char * Method, int KLSH,int L,int Kcube, int M, int probes,const char * outputfileName,bool flag){

    outputFile.open(outputfileName, std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open the output file." << std::endl;
        return;
    }
    //Constructor of Kmeans
    K = K_;
    MyClusters  = new Cluster *[K];
    random_device rd;  
    mt19937 gen(rd()); 
    uniform_int_distribution<> distrib(0, GetTrainNumber());
    //Randomly pick the first Centroid
    int NewCentroidIndex = distrib(gen);
    MyClusters[0] = new Cluster(NewCentroidIndex);
    //Setting the Cluster of datapoint using the position in dataset
    SetCluster(NewCentroidIndex,0);
    Initialize();           //KMEans++
    clock_t start, end;
    double Time = 0;

    start = clock();
    if (strcmp(Method,"Classic") == 0 ){
        outputFile << "Algorithm: Lloyds\n";
        Lloyds();
    }
    else if (strcmp(Method,"LSH") == 0 || strcmp(Method,"Hypercube") == 0 ){
        outputFile << "Algorithm: Range Search " << Method << "\n";
        RangeSearch(KLSH,L,Kcube,M,probes,Method);
    }
    else{
        outputFile << "No such method" << Method << "\n";
        return;
    }
    end = clock();
    Time = double(end - start) / double(CLOCKS_PER_SEC);
    
    for (int i =0 ; i< K;i++){
        outputFile << "Cluster-" << i+1 << " {size: " << MyClusters[i]->GetSize()<< " centroid: ";
        MyClusters[i]->PrintCentroid();
        outputFile << " }\n";
    }
    outputFile <<"clustering_time: " << Time <<"\n";
    Silhouette();
    if (flag){
        outputFile << "\n\n\n\n\n##################################################################\n\n\n";
        for (int i = 0;i<K ; i ++ )
            MyClusters[i]->DisplayFullInfo();
    }
    //Calculate Objective Function
    double ObjectiveFunction = 0;
    for (int i = 0 ; i< GetTrainNumber();i++){
        ObjectiveFunction += powf(Euclidean(MyClusters[GetCluster(i)]->GetCentroid(),GetRepresenation(i),GetDimension()),2)/GetTrainNumber();
    }
    outputFile << "Objective Function Value: " << ObjectiveFunction << "\n";

    for (int i = 0 ; i < GetTrainNumber() ; i++)
        SetCluster(i,-1);
    outputFile.close();
}
KMeans :: KMeans(int K_,char * Method, int KLSH,int L,int Kcube, int M, int probes,const char * outputfileName,bool flag,string InputFile){
    //Overloaded constructor for reduced dataset that also gets the input file to the original dataset

    outputFile.open(outputfileName, std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open the output file." << std::endl;
        return;
    }
    //Constructor of Kmeans
    K = K_;
    MyClusters  = new Cluster *[K];
    random_device rd;  
    mt19937 gen(rd()); 
    uniform_int_distribution<> distrib(0, GetTrainNumber());
    //Randomly pick the first Centroid
    int NewCentroidIndex = distrib(gen);
    MyClusters[0] = new Cluster(NewCentroidIndex);
    //Setting the Cluster of datapoint using the position in dataset
    SetCluster(NewCentroidIndex,0);
    Initialize();           //KMEans++
    clock_t start, end;
    double Time = 0;

    start = clock();
    if (strcmp(Method,"Classic") == 0 ){
        outputFile << "Algorithm: Lloyds\n";
        Lloyds();
    }
    else if (strcmp(Method,"LSH") == 0 || strcmp(Method,"Hypercube") == 0 ){
        outputFile << "Algorithm: Range Search " << Method << "\n";
        RangeSearch(KLSH,L,Kcube,M,probes,Method);
    }
    else{
        outputFile << "No such method" << Method << "\n";
        return;
    }
    end = clock();
    Time = double(end - start) / double(CLOCKS_PER_SEC);
    for (int i =0 ; i< K;i++){      //Print clusters in reduced space
        outputFile << "Cluster-" << i+1 << " {size: " << MyClusters[i]->GetSize()<< " centroid: ";
        MyClusters[i]->PrintCentroid();
        outputFile << " }\n";
    }
    outputFile <<"clustering_time: " << Time <<"\n"; 
    Silhouette();      //Calculate Silhouette for reduced space 
    if (flag){
        outputFile << "\n\n\n\n\n##################################################################\n\n\n";
        for (int i = 0;i<K ; i ++ )
            MyClusters[i]->DisplayFullInfo();
    }
    //Calculate Objective Function
    double ObjectiveFunction = 0;
    for (int i = 0 ; i< GetTrainNumber();i++){
        ObjectiveFunction += powf(Euclidean(MyClusters[GetCluster(i)]->GetCentroid(),GetRepresenation(i),GetDimension()),2)/GetTrainNumber();
    }
    outputFile << "Objective Function Value: " << ObjectiveFunction << "\n";    //Print objective function in reduced space
    outputFile << "For reduced dataset we have : \n";
    SilhouetteReduced(InputFile);    //Calculate Silhouette for original space this function also loads the original dataset
    
    //Calculate Objective Function
    ObjectiveFunction = 0;
    for (int i = 0 ; i< GetTrainNumber();i++){
        ObjectiveFunction += powf(Euclidean(MyClusters[GetCluster(i)]->GetCentroid(),GetRepresenation(i),GetDimension()),2)/GetTrainNumber();
    }
    outputFile << "Objective Function Value: " << ObjectiveFunction << "\n";
    for (int i =0 ; i< K;i++){
        outputFile << "Cluster-" << i+1 << " {size: " << MyClusters[i]->GetSize()<< " centroid: ";
        MyClusters[i]->PrintCentroid();
        outputFile << " }\n";
    }
    for (int i = 0 ; i < GetTrainNumber() ; i++)
        SetCluster(i,-1);
    outputFile.close();
}
KMeans :: ~KMeans(){
    for (int i = 0 ; i < K ; i ++)
        delete MyClusters[i];
    delete []MyClusters;
}

void KMeans :: Initialize(){
    double * AllEuclideans = new double[GetTrainNumber()];      //D(i)
    int DIMENSION = GetDimension();
    double Max = -1;        //Stores Max D(i)
    for (int NumberOfCentroids =1;NumberOfCentroids < K;NumberOfCentroids++){
        for (int i = 0 ; i < GetTrainNumber() ;i++){
            if (GetCluster(i) != -1){
                //Already in cluster, 
                AllEuclideans[i] = 0;
                continue;
            }
            double Min = (double)MAXSIZE;
            //We will now calculate D(i)
            for ( int j = 0 ; j < NumberOfCentroids ;j++){
                //Check distances from all centroids and pick the smaller one
                double e = Euclidean(MyClusters[j]->GetCentroid(),GetRepresenation(i),DIMENSION);
                if (e < Min)
                    Min = e;
                else if (e > Max)
                    Max = e;
            }
            AllEuclideans[i] = Min;
        }
        double Accumulator = 0;     //Accumulator will store P(n-t)
        for (int i = 0 ; i < GetTrainNumber() ; i ++){
            AllEuclideans[i] /= Max;        //Normalize
            AllEuclideans[i] = pow(AllEuclideans[i],2);     //Square
            Accumulator += AllEuclideans[i];        
        }
        //Pick a random float
        std::uniform_real_distribution<float> t_distribution(0.0,Accumulator);
        std::random_device rd_t;
        std::mt19937 t_generator(rd_t());
        double Result = t_distribution(t_generator);
        double temp = 0,temp2;          // We want temp <= Result <= temp2
        //Each time temp and temp2 will icnrease by D(i) untill this condition is fullfilled
        //The i will then be our new Centroid
        for (int i = 0 ; i < GetTrainNumber();i++){
            if (GetCluster(i) != -1)
                continue;
            temp2 = temp + AllEuclideans[i];
            if (Result >= temp && Result <= temp2){
                MyClusters[NumberOfCentroids] = new Cluster(i);
                SetCluster(i,NumberOfCentroids);
                break;
            }
            temp += AllEuclideans[i];
        }
    }
    delete []AllEuclideans;
}

void KMeans :: Lloyds(void){
    int DIMENSION = GetDimension();
    int Position;
    double Min;
    bool SmtChanged;
    do
    {
        SmtChanged = false;
        //SmthChanged will be set to true when an insertion has occured
        //We loop whilw SmtChanged
        for (int i = 0; i < GetTrainNumber() ; i++){
            
            Position=-1;
            Min = static_cast<double>(MAXSIZE);
            for (int j = 0 ; j < K ; j++ ){
                //Check euclideans from all centers and hold position from the closer one
                double e = Euclidean(MyClusters[j]->GetCentroid(),GetRepresenation(i),DIMENSION);
                if (e < Min){
                    Min = e;
                    Position = j;
                }   
            }
            if (Position == GetCluster(i) )     //Position is each current  cluster, do nothing
                continue;
            SmtChanged = true;  // We have an insertion
            if (GetCluster(i) != -1)       
             // If value different than default, the point was in a cluster before, delete it from this cluster
                MyClusters[GetCluster(i)]->DeleteMember(i);
            MyClusters[Position]->InsertMember(i);
            SetCluster(i,Position);
            
        }
    } while (SmtChanged);

}

void KMeans :: RangeSearch(int KLSH,int L,int KCube,int M,int probes,char * Method){
    int DIMENSION = GetDimension();
    LSH * MyLsh;
    RandomProjection * MyCube;
    if (strcmp(Method,"LSH") == 0 ){
        MyLsh = new LSH(KLSH,L);
        MyLsh->Train();
    }
    else{
        MyCube = new RandomProjection(KCube,M,probes);
        MyCube->Train();
    }
    int i,j;
    double R = static_cast<double>(MAXSIZE);
    //Calculate R
    for (i = 0 ; i < K; i++){
        for (j = 0 ; j < K ;j ++){
            if (i == j)
                continue;
            double e = Euclidean(MyClusters[i]->GetCentroid(),MyClusters[j]->GetCentroid(),DIMENSION);
            if ( e < R)
                R = e;
        }
    }
    R/=4;      // We divide by 4, because we have a do while loop and we *2 in the beggining
    bool SmtChanged;
    int epochs = 1;
    do
    {
        SmtChanged = false;
        
        if (strcmp(Method,"LSH") == 0 ){
            if (R < 1600)
                R*=2;
            if (R>1600)
                R = 1600;
        }
        else{
            if (R<3000)
                R*=2;
        }
        vector <int> * Neighbors;
        Neighbors = new vector <int>[K];
        //Get neighbors of each centrod
        for (int i = 0 ; i < K ; i ++){
            uint8_t * temp = MyClusters[i]->CentroidAsBytes();
            if (strcmp(Method,"LSH") == 0 )
                Neighbors[i] = MyLsh->RangeSearch(R,temp);
            else
                Neighbors[i] = MyCube->RangeSearch(R,temp);
            delete []temp;
        }
        for (int i = 0 ; i < K ; i++){
            if (Neighbors[i].size() == 0 )
                continue;
            for (int j = 0 ; j <(int) Neighbors[i].size(); j++){
                //For all neighbors that range search returned
                int Pos = GetCluster(Neighbors[i][j]);
                if (Pos != -1)      //The point is already in a cluster, do nothing
                    continue;
                Pos = i;
                for (int x = 0 ; x < K ; x++){
                    //For all other clusters x
                    if (x == Pos)
                        continue;
                    if(find(Neighbors[x].begin(), Neighbors[x].end(), Neighbors[i][j]) != Neighbors[x].end()) {
                        //If it belongs to the neighbors of cluster x
                        if (Euclidean(MyClusters[x]->GetCentroid(),GetRepresenation(Neighbors[i][j]),DIMENSION) < Euclidean(MyClusters[Pos]->GetCentroid(),GetRepresenation(Neighbors[i][j]),DIMENSION)){
                            //Find the closest of the clusters
                            Pos = x;
                        }
                    }
                }
                MyClusters[Pos]->InsertMemberRangeSearch(Neighbors[i][j]);
                SetCluster(Neighbors[i][j],Pos);
                SmtChanged = true;
            }
            
        }
        delete []Neighbors;
        for (int i = 0 ; i < K ; i ++ ) // Update centroids
            MyClusters[i]->Update();
        epochs++;
    } while ((R < R_THRESHHOLD || SmtChanged) && epochs < 4);
    if (strcmp(Method,"LSH") == 0 ){
       delete MyLsh;
    }
    else{
        delete MyCube;
    }
    for (int i = 0 ; i < GetTrainNumber();i++){ 
        //Use Lloyds for the datapoints with no cluster
        if (GetCluster(i) != -1)
            continue;
        double Min = static_cast<double>(MAXSIZE);
        int MinPos = -1;
        for (int j = 0; j < K;j++){
            double  e = Euclidean(MyClusters[j]->GetCentroid(),GetRepresenation(i),DIMENSION);
            if ( e < Min){
                Min = e;
                MinPos = j;
            }
        }
        MyClusters[MinPos]->InsertMember(i);
        SetCluster(i,MinPos);
    }
}


void KMeans :: Silhouette(void){
    int DIMENSION = GetDimension();
    // Simple implementation of metric
    double * A = new double [GetTrainNumber()], * B =new double [GetTrainNumber()], * S = new double[GetTrainNumber()] ;
    double * SAvergae = new double [K];
    double STotal = 0;
    for (int i = 0 ; i < K ; i++)
        SAvergae[i] = 0;
    for (int i = 0 ; i < GetTrainNumber();i++){
        A[i] = GetMean(i,GetCluster(i));
        double Min = static_cast <double> (MAXSIZE);
        int MinPos = -1;
        for (int j = 0; j < K ; j++){
            if (GetCluster(i) == j )
                continue;
            double e = Euclidean(MyClusters[j]->GetCentroid(),GetRepresenation(i),DIMENSION);
            if (e < Min){
                MinPos = j;
                Min = e;
            }
        }
        B[i] = GetMean(i,MinPos);
        if (A[i] < B[i])
            S[i] = 1  - A[i]/B[i];
        else if (A[i] > B[i]){
            S[i] = B[i]/A[i] -1;
        }
        else
            S[i] = 0;
        SAvergae[GetCluster(i)] += S[i]/(double)MyClusters[GetCluster(i)]->GetSize();
        STotal += S[i];
    }
    STotal/=(double)GetTrainNumber();
    outputFile << "Silhouette: [ ";
    for (int i = 0 ; i< K; i++)
        outputFile <<SAvergae[i] <<", ";
    outputFile << STotal << " ]\n";
    delete []A;
    delete []B;
    delete []S;
    delete []SAvergae;
}

double KMeans :: GetMean(int i,int ClusterIndex){
    
    int DIMENSION = GetDimension();
    //Function that get Mean distance from point i to all the members that belong to cluster with this index
    double ToReturn = 0;
    
    for (int j = 0 ; j < MyClusters[ClusterIndex]->GetSize();j++){
        if (MyClusters[ClusterIndex]->GetMember(j) == i)
            continue;
        ToReturn+= Euclidean(GetRepresenation(MyClusters[ClusterIndex]->GetMember(j)),GetRepresenation(i),DIMENSION);
    }
    
    return ToReturn/(double)MyClusters[ClusterIndex]->GetSize();
}

void KMeans :: SilhouetteReduced(string inputFileReduced){     //Same as Silhouette but also loads the original dataset 
    int ClusterPerPoint[GetTrainNumber()];
    for (int i = 0 ; i < GetTrainNumber() ; i++){
        ClusterPerPoint[i] = GetCluster(i);     //Store the cluster of each point so we can assigng it in the original dataset since when we load it it will be set to -1
    }
    if (ReadTrainData(inputFileReduced) == ERROR)   //Load original dataset
        return;
    int DIMENSION = GetDimension();
    for (int i = 0 ; i < GetTrainNumber();i++)
        SetCluster(i,ClusterPerPoint[i]);
    // Simple implementation of metric
    for (int i = 0 ; i < K ; i ++ )     // Update centroids
        MyClusters[i]->UpdateFromReduced();

    // Same preocedure as Silhouette
    double * A = new double [GetTrainNumber()], * B =new double [GetTrainNumber()], * S = new double[GetTrainNumber()] ;
    double * SAvergae = new double [K];
    double STotal = 0;
    
    
    
    for (int i = 0 ; i < K ; i++)
        SAvergae[i] = 0;
    
    for (int i = 0 ; i < GetTrainNumber();i++){
        A[i] = GetMean(i,GetCluster(i));
        double Min = static_cast <double> (MAXSIZE);
        int MinPos = -1;
        for (int j = 0; j < K ; j++){
            if (GetCluster(i) == j )
                continue;
            double e = Euclidean(MyClusters[j]->GetCentroid(),GetRepresenation(i),DIMENSION);
            if (e < Min){
                MinPos = j;
                Min = e;
            }
        }
        B[i] = GetMean(i,MinPos);
        if (A[i] < B[i])
            S[i] = 1  - A[i]/B[i];
        else if (A[i] > B[i]){
            S[i] = B[i]/A[i] -1;
        }
        else
            S[i] = 0;
        SAvergae[GetCluster(i)] += S[i]/(double)MyClusters[GetCluster(i)]->GetSize();
        STotal += S[i];
        
    }
    
    STotal/=(double)GetTrainNumber();
    outputFile << "Silhouette: [ ";
    for (int i = 0 ; i< K; i++)
        outputFile <<SAvergae[i] <<", ";
    outputFile << STotal << " ]\n";
    cout << "Return Value: " << STotal << endl; //Return value for python script
    
    delete []A;
    delete []B;
    delete []S;
    delete []SAvergae;
}
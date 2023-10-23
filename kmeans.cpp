#include "kmeans.h"
#include "ReadTrainData.h"
#include <random>
#include <cstring>
#include "hFunc.h"
#include <algorithm>
#include "lsh.h"
#include "RandomProjection.h"
#define TABLE_SIZE 5
#define MAXSIZE 1215752202
#define DIMENSION 784
#define WINDOW 5
#define ERROR -1
#define R_THRESHHOLD 2000
static int times=0;

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
    private : 
        double * Centroid;
        vector <int> Members;
    public : 
        Cluster(int  NewCentroid){
            Centroid = new double [DIMENSION];
            uint8_t * temp = GetRepresenation(NewCentroid);
            for (int i = 0 ; i < DIMENSION ;i ++)
                Centroid[i] = (double) temp[i];
            Members.push_back(NewCentroid);
        }
        ~Cluster(){};
        void InsertMember(int NewMember){
            for (int i = 0 ; i < DIMENSION; i++)
                Centroid[i] = ( (Centroid[i]*Members.size()) + (double)GetRepresenation(NewMember)[i]) / (Members.size()+1);
            Members.push_back(NewMember);
        }
        double * GetCentroid(void){
            return Centroid;
        }
        void DeleteMember(int ToDelete){
            times++;
            Members.erase(std::remove(Members.begin(), Members.end(), ToDelete), Members.end());
            for (int i = 0 ; i < DIMENSION; i++){
                Centroid[i] = ( (Centroid[i]* (Members.size()+1)) - (double)GetRepresenation(ToDelete)[i]) / (Members.size());
                if (Centroid[i] <0)
                    Centroid[i] = 0;
            }
        }
        uint8_t * CentroidAsBytes(void){
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
        void PrintCentroid(void){
            cout << "[ ";
            for (int i = 0 ; i < DIMENSION ; i++){
                if (i == DIMENSION -1)
                    cout << Centroid[i] << " ]\n";
                else
                    cout << Centroid[i] << ", ";
            }
        }

};


KMeans :: KMeans(int K_,char * Method, int KLSH,int L,int Kcube, int M, int probes){
    K = K_;
    MyClusters  = new Cluster *[K];
    random_device rd;  
    mt19937 gen(rd()); 

    uniform_int_distribution<> distrib(0, GetTrainNumber());
    int NewCentroidIndex = distrib(gen);
    MyClusters[0] = new Cluster(NewCentroidIndex);
    SetCluster(NewCentroidIndex,0);
    Initialize();
    clock_t start, end;
    double Time = 0;

    start = clock();
    if (strcmp(Method,"Classic") == 0 ){
        cout << "Algorithm: Lloyds\n";
        Lloyds();
    }
    else if (strcmp(Method,"LSH") == 0 || strcmp(Method,"Hypercube") == 0 ){
        cout << "Algorithm: Range Search " << Method << "\n";
        RangeSearch(KLSH,L,Kcube,M,probes,Method);
    }
    else
        cout << "No such method" << Method << "\n";
    end = clock();
    Time = double(end - start) / double(CLOCKS_PER_SEC);
    
    for (int i =0 ; i< K;i++){
        cout << "Cluster-" << i+1 << " {size: " << MyClusters[i]->GetSize()<< " centroid: ";
        MyClusters[i]->PrintCentroid();
        cout << " }\n";
    }
    cout <<"clustering_time: " << Time <<"\n";

    //for (int i = 0;i< K ; i ++ ){
        ///cout << "For cluster " << i << "\n";
        for (int j = 0 ; j < 300; j++){
            for (int x = 0 ; x < 28 ; x++){
                for (int y = 0 ; y < 28 ; y++){
                    if ((int)GetRepresenation(MyClusters[0]->GetMember(j))[28*x + y] > 128) {
                        std::cout << "#"; // Print a character for dark pixels
                    } else {
                        std::cout << " "; // Print a space for light pixels
                    }
                }
                cout << "\n";
            }
            cout << "\n\n";
        }
    //}

    Silhouette();
    for (int i = 0 ; i < GetTrainNumber() ; i++)
        SetCluster(i,-1);
}
KMeans :: ~KMeans(){
    for (int i = 0 ; i < K ; i ++)
        delete MyClusters[i];
    delete []MyClusters;
}

void KMeans :: Initialize(){
    double * AllEuclideans = new double[GetTrainNumber()];
    double Max = -1;
    for (int NumberOfCentroids =1;NumberOfCentroids < K;NumberOfCentroids++){
        for (int i = 0 ; i < GetTrainNumber() ;i++){
                
            if (GetCluster(i) != -1){
                AllEuclideans[i] = 0;
                continue;
            }
            double Min = (double)MAXSIZE;
            for ( int j = 0 ; j < NumberOfCentroids ;j++){
                double e = Euclidean(MyClusters[j]->GetCentroid(),GetRepresenation(i),DIMENSION);
                if (e < Min)
                    Min = e;
                else if (e > Max)
                    Max = e;
            }
            AllEuclideans[i] = Min;
        }
        double Accumulator = 0;
        for (int i = 0 ; i < GetTrainNumber() ; i ++){
            AllEuclideans[i] /= Max;
            AllEuclideans[i] = pow(AllEuclideans[i],2);
            Accumulator += AllEuclideans[i];
        }
        std::uniform_real_distribution<float> t_distribution(0.0,Accumulator);
        std::random_device rd_t;
        std::mt19937 t_generator(rd_t());
        double Result = t_distribution(t_generator);
        double temp = 0,temp2;
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
}

void KMeans :: Lloyds(void){
    int Position;
    double Min;
    bool SmtChanged ;
    do
    {
        SmtChanged = false;
        for (int i = 0; i < GetTrainNumber() ; i++){
            
            Position=-1;
            Min = static_cast<double>(MAXSIZE);
            for (int j = 0 ; j < K ; j++ ){
                
                double e = Euclidean(MyClusters[j]->GetCentroid(),GetRepresenation(i),DIMENSION);
                if (e < Min){
                    Min = e;
                    Position = j;
                }   
            }
            if (Position == GetCluster(i) )
                continue;
            SmtChanged = true;
            if (GetCluster(i) != -1)
                MyClusters[GetCluster(i)]->DeleteMember(i);
            MyClusters[Position]->InsertMember(i);
            SetCluster(i,Position);
            
        }
    } while (SmtChanged);

}

void KMeans :: RangeSearch(int KLSH,int L,int KCube,int M,int probes,char * Method){
    
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
    for (i = 0 ; i < K; i++){
        for (j = 0 ; j < K ;j ++){
            if (i == j)
                continue;
            double e = Euclidean(MyClusters[i]->GetCentroid(),MyClusters[j]->GetCentroid(),DIMENSION);
            if ( e < R)
                R = e;
        }
    }
    R/=4;
    bool SmtChanged;
    do
    {
        R*=200;
        SmtChanged = false;
        vector <int> * Neighbors;
        Neighbors = new vector <int>[K];
        for (int i = 0 ; i < K ; i ++){
            
            uint8_t * temp = MyClusters[i]->CentroidAsBytes();
            if (strcmp(Method,"LSH") == 0 )
                Neighbors[i] = MyLsh->RangeSearch(R,temp);
            else
                Neighbors[i] = MyCube->RangeSearch(R,temp);
            delete temp;
        }
        for (int i = 0 ; i < K ; i++){
            if (Neighbors->size() == 0 )
                continue;
            for (int j = 0 ; j <(int) Neighbors[i].size(); j++){
                int Pos = GetCluster(Neighbors[i][j]);
                if (Pos == -1)
                    Pos = i;
                for (int x = 0 ; x < K ; x++){
                    if (x == Pos)
                        continue;
                    if(find(Neighbors[x].begin(), Neighbors[x].end(), Neighbors[i][j]) != Neighbors[x].end()) {
                        if (Euclidean(MyClusters[x]->GetCentroid(),GetRepresenation(Neighbors[i][j]),DIMENSION) < Euclidean(MyClusters[Pos]->GetCentroid(),GetRepresenation(Neighbors[i][j]),DIMENSION)){
                            Pos = x;
                        }
                    }
                }
                if (Pos == GetCluster(Neighbors[i][j]))
                    continue;
                if (GetCluster(Neighbors[i][j]) != -1)
                     MyClusters[GetCluster(Neighbors[i][j])]->DeleteMember(Neighbors[i][j]);
                MyClusters[Pos]->InsertMember(Neighbors[i][j]);
                SetCluster(Neighbors[i][j],Pos);
                SmtChanged = true;
            }
        }
    } while ( (R < R_THRESHHOLD || SmtChanged ) && R < 4000000000 );

    for (int i = 0 ; i < GetTrainNumber();i++){
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
    cout << "Silhouette: [ ";
    for (int i = 0 ; i< K; i++)
        cout <<SAvergae[i] <<", ";
    cout << STotal << " ]\n";
}

double KMeans :: GetMean(int i,int ClusterIndex){
    double ToReturn = 0;
    for (int j = 0 ; j < MyClusters[ClusterIndex]->GetSize();j++){
        if (MyClusters[ClusterIndex]->GetMember(j) == i)
            continue;
        ToReturn+= Euclidean(GetRepresenation(MyClusters[ClusterIndex]->GetMember(j)),GetRepresenation(i),DIMENSION);
    }
    return ToReturn/(double)MyClusters[ClusterIndex]->GetSize();
}

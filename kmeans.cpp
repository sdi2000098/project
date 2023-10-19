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
class Cluster{
    private : 
        uint8_t * Centroid;
        vector <int> Members;
    public : 
        Cluster(int  NewCentroid){
            Centroid = GetRepresenation(NewCentroid);
            Members.push_back(NewCentroid);
        }
        ~Cluster(){};
        void InsertMember(int NewMember){
            Members.push_back(NewMember);
            Update();
        }
        uint8_t * GetCentroid(void){
            return Centroid;
        }
        void DeleteMember(int ToDelete){
            for (int i = 0 ; i < Members.size() ; i++){
                if (Members[i] == ToDelete){
                    Members.erase(Members.begin() + i);
                    break;
                }
            }
            Update();
        }
        void Update(void){
            for(int i = 0 ; i < DIMENSION ; i ++){
                double Accumulator = 0;
                for (int j=0; j<Members.size();j++ )
                    Accumulator += GetRepresenation(Members[j])[i];
                Centroid[i] = Accumulator/Members.size();
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
    SetChecked(NewCentroidIndex);
    Initialize();
    if (strcmp(Method,"Classic") == 0 )
        Lloyds();
    else if (strcmp(Method,"LSH") == 0 || strcmp(Method,"Hypercube") == 0 )
        RangeSearch(KLSH,L,Kcube,M,probes,Method);
    else
        cout << "No such method" << Method << "\n";
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
            if (GetChecked(i)){
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
            if (GetChecked(i))
                continue;
            temp2 = temp + AllEuclideans[i];
            if (Result >= temp && Result <= temp2){
                SetChecked(i);
                MyClusters[NumberOfCentroids] = new Cluster(i);
                break;
            }
            temp += AllEuclideans[i];
        }
    }
}

void KMeans :: Lloyds(void){
    int Position;
    double Min;
    for (int i = 0 ; i < GetTrainNumber();i++){
        Position=-1;
        Min = static_cast<double>(MAXSIZE);
        for (int j = 0 ; j < K ; j ++){
            double e = Euclidean(MyClusters[j]->GetCentroid(),GetRepresenation(i),DIMENSION);
                if (e < Min){
                    Min = e;
                    Position = j;
                }
        }
        MyClusters[Position]->InsertMember(i);
    }
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
            MyClusters[GetCluster(i)]->DeleteMember(i);
            MyClusters[Position]->InsertMember(i);
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
        for (j = 0 ; j < K ;j ++)
            if (i == j)
                continue;
            double e = Euclidean(MyClusters[i]->GetCentroid(),MyClusters[j]->GetCentroid(),DIMENSION);
            if ( e < R)
                R = e;
    }
    R/=2;
    bool * SmtChanged  = new bool[K];
    do
    {
        vector <int> * Neighbors;
        Neighbors = new vector <int>[K];
        for (int i = 0 ; i < K ; i ++){
            SmtChanged[i] = false;
            if (strcmp(Method,"LSH") == 0 )
                Neighbors[i] = MyLsh->RangeSearch(R,MyClusters[i]->GetCentroid());
            else
                Neighbors[i] = MyCube->RangeSearch(R,MyClusters[i]->GetCentroid());
        }
        for (int i = 0 ; i < K ; i++){
            if (Neighbors->size() == 0 )
                continue;
            for (int j = 0 ; j < Neighbors[i].size(); j++){
                if (GetChecked(Neighbors[i][j]))
                    continue;
                int Pos = i;
                for (int x = 0 ; x < K ; x++){
                    if (x == i)
                        continue;
                    if(find(Neighbors[x].begin(), Neighbors[x].end(), Neighbors[i][j]) != Neighbors[x].end()) {
                        if (Euclidean(GetRepresenation(Neighbors[i][j]),MyClusters[x]->GetCentroid(),DIMENSION) < Euclidean(GetRepresenation(Neighbors[i][j]),MyClusters[i]->GetCentroid(),DIMENSION))
                            Pos = x;
                    }
                }
                if ( Pos == i )
                    continue;
                MyClusters[i]->DeleteMember(Neighbors[i][j]);
                MyClusters[Pos]->InsertMember(Neighbors[i][j]);
                SetChecked(Neighbors[i][j]);
                SmtChanged[i] = true;
            }
        }
    } while (R < R_THRESHHOLD );
    

}
#include "RandomProjection.h"
#include "hFunc.h"
#include <random>
#include "ReadTrainData.h"

#define DIMENSION 784
#define WINDOW 5
#define MAXSIZE 1215752202

using namespace std;

double EuclideanHypercube(uint8_t * x, uint8_t * y, int D){
    double ToReturn = 0;
    for (int i = 0 ; i < D;i++)
        ToReturn += pow(x[i] - y[i] , 2);
    
    return sqrt(ToReturn);
}

int HammingDistance(uint8_t * x, uint8_t *y, int dimension){
    int ToReturn =0;
    for (int i = 0 ; i < dimension ; i ++)
        if (x[i] != y[i])
            ToReturn++;
    
    return ToReturn;
}

class FValues{

    private :
        uint8_t Value;
        int Input;
    public :
        FValues * Next;
        FValues(int input,int value){
            Input = input;
            Value = value;
            Next =NULL;
        }
        int GetInput(void){
            return Input;
        }
        uint8_t GetValue(void){
            return Value;
        }

};

class HypercubePoint{
    private :
        uint8_t * Pixels;
        uint8_t * Representation;
        int Position;
    public :
        HypercubePoint * Next;

        HypercubePoint(uint8_t * pixels,int RepresentationDimension,int Pos){
            Pixels = pixels;
            Representation = new uint8_t[RepresentationDimension];
            Next = NULL;
            Position = Pos;
        }
        void Insert(uint8_t value,int Position){
            Representation[Position] = value;
        }
        ~HypercubePoint(){
            delete Representation;
        }
        uint8_t * GetPixels(void){
            return Pixels;
        }
        uint8_t * GetRepresentation(void){
            return Representation;
        }
        int GetPosition(void){
            return Position;
        }
};

class FunctionF {
    
    private :
        hFunction * MyH;
        FValues * MyValues;
    public :
        FunctionF(){
            MyH = new hFunction(DIMENSION,WINDOW);
            MyValues = NULL;
        }

        u_int8_t FindValue(u_int8_t * p){
            int HValue = MyH->operator()(p);
            if (HValue < 0 )
                HValue *=-1;
            if (MyValues == NULL){
                random_device rd;  
                mt19937 gen(rd()); 
                uniform_int_distribution<> distrib(0, 1);
                MyValues = new FValues(HValue,distrib(gen));
                return MyValues->GetValue();
            }
            FValues * temp = MyValues,* prev = MyValues;
            while (temp != NULL)
            {
                if (temp->GetInput() == HValue)
                    return temp->GetValue();
                prev = temp;
                temp = temp->Next;
            }
            random_device rd;  
            mt19937 gen(rd()); 
            uniform_int_distribution<> distrib(0, 1);
            prev->Next = new FValues(HValue,distrib(gen));
            return prev->Next->GetValue();
        }
        ~FunctionF(){
            delete MyH;
            FValues * temp = MyValues,* next = MyValues;
            while (next != NULL)
            {
                temp = next;
                next= next->Next;
                delete temp;
            }
            
        }
};

RandomProjection ::RandomProjection(int k, int m, int probes){
    K = k;
    M=m;
    Probes = probes;
    MyF = new FunctionF *[K];
    for (int i = 0 ; i < K ; i ++)
        MyF[i] = new FunctionF();
    
}

RandomProjection :: ~RandomProjection(){
    for (int i = 0 ; i < K ; i ++)
        delete MyF[i];
    delete []MyF;
}


void RandomProjection ::SetTrainData(uint8_t * p,int Position){
    HypercubePoint * NewPoint = new HypercubePoint(p,K,Position);
    for (int i = 0 ; i < K ; i ++)
        NewPoint->Insert(MyF[i]->FindValue(p),i);
    if( this->TrainData == NULL){
        TrainData = NewPoint;
        LastInsert = TrainData;
    }
    LastInsert->Next=NewPoint;
    LastInsert = LastInsert->Next;
}

void RandomProjection ::Train(void){
    int limit = GetTrainNumber();
    for (int i = 0;i<limit;i++)
        SetTrainData(GetRepresenation(i),i);
}

int RandomProjection :: NearestNeighbour(uint8_t * Query,int Hamming){
    uint8_t * QuereyRepresentation = new uint8_t[this->K];
    for (int i = 0 ; i < K ; i ++)  
        QuereyRepresentation[i] = MyF[i]->FindValue(Query);
    int  MinPos = -1;
    double MinSize = (double)MAXSIZE; 
    HypercubePoint * temp = TrainData;
    while (temp != NULL)
    {
        if (!GetChecked(temp->GetPosition()) && HammingDistance(QuereyRepresentation,temp->GetRepresentation(),this->K) <= Probes){
            double e = EuclideanHypercube(Query,temp->GetPixels(),DIMENSION);
            if (e < MinSize){
                MinSize = e;
                MinPos = temp->GetPosition();
            }
        }
        temp = temp->Next;
    }
    return MinPos;
    
}


int * RandomProjection :: KNN(int K,uint8_t * Query){
    int * ToReturn = new int[K];
    for (int i = 0 ; i < K; i++)
        ToReturn[i] = -1;
    int i = 0;
    for (int CurrentHamming = 0; CurrentHamming <= Probes;CurrentHamming++){
        int Result;
        do
        {
            if (i == K){
                for (int i = 0 ; i < K ; i++)
                    SetUnchecked(ToReturn[i]);
                CurrentHamming = Probes +1;
                break;
            }
            
            Result = NearestNeighbour(Query,CurrentHamming);
            if (Result != -1){
                SetChecked(Result);
                ToReturn[i++] = Result;
            }
        } while (Result != -1);  
    }
    cout << "Printing " << i << " nearest neighbours :\n";
    for (int j = 0 ; j < i ; j++){
        cout << "Neighbour - " << j << " position : " << ToReturn[j] << " distance : " ;
        cout << EuclideanHypercube(Query,GetRepresenation(ToReturn[j]),DIMENSION) << "\n";
        SetUnchecked(ToReturn[j]);
    }
    return ToReturn;
}
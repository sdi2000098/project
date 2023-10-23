#include "RandomProjection.h"
#include "hFunc.h"
#include <random>
#include "ReadTrainData.h"
#include <bits/stdc++.h>

#define DIMENSION 784
#define WINDOW 5
#define MAXSIZE 1215752202

using namespace std;


//For the implementation of the hypercube, each point is converted to a K-dimension array of unsigned bytes
//these bytes in fact will only store 0 or 1, but we decided to use bytes and not boolean bits for easier
//computations and understanding. Furthermore using boolean we would not even get smaller memory occupation
//since boolean are also a byte long


int HammingDistance(long long int x, long long int y) {
    long long int xor_result = x ^ y;
    int distance = 0;
    
    // Count the number of set bits (1s) in the XOR result
    while (xor_result) {
        distance += xor_result & 1;
        xor_result >>= 1;
    }
    
    return distance;
}


class FValues{
    //Lazy way to store values of F function
    //Every time we get an input, we randomly assign it a value 0 or 1
    //We store those values and whenever same input is given we return the value
    //If input is new, we creaqte a new fValue
    //Fvalue also follow the structure of linked lists
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
    //Class to store HyperCube points, each point consists of the image representation of bytes-pixeles (Pixels),
    //the representation of the projection of the image (Representation) and position in the dataset
    private :
        uint8_t * Pixels;
        unsigned long long int Representation = 0;
        int Position;
    public :
        HypercubePoint * Next;

        HypercubePoint(uint8_t * pixels,int RepresentationDimension,int Pos){
            //Given the dimension d' we allocate memory for the Representation and construct hypercube point
            Pixels = pixels;
            Next = NULL;
            Position = Pos;
        }
        void Insert(uint8_t value,int Position){
            //Insert 0-1 value to Position in the Representation
            if (value == 1)
                Representation |= (1LL << Position);;
        }
        ~HypercubePoint(){
        }
        //Some Getters
        uint8_t * GetPixels(void){
            return Pixels;
        }
        unsigned long long int GetRepresentation(void){
            return Representation;
        }
        int GetPosition(void){
            return Position;
        }
};

class FunctionF {
    //Function F consists of a unique LSH function and unique FValues
    private :
        hFunction * MyH;
        FValues * MyValues;
    public :
        FunctionF(){
            MyH = new hFunction(DIMENSION,WINDOW);
            MyValues = NULL;
        }

        u_int8_t FindValue(u_int8_t * p){
            //Given a point this function returns 0 or 1 based on fvalues and h(p)
            int HValue = MyH->operator()(p);
            if (MyValues == NULL){
                //We will create our first fValue
                random_device rd;  
                mt19937 gen(rd()); 
                uniform_int_distribution<> distrib(0, 1);
                MyValues = new FValues(HValue,distrib(gen));
                return MyValues->GetValue();
            }
            FValues * temp = MyValues,* prev = MyValues;
            //Iterate through fValues
            while (temp != NULL)
            {
                if (temp->GetInput() == HValue)
                    //If fvalue's input is same as hValue, we just return the fvalue's value
                    return temp->GetValue();
                prev = temp;
                temp = temp->Next;
            }
            //If we reached here, this means that there is no fvalue for the new hvalue, we create one
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
    //Simple constructor
    K = k;
    M=m;
    Probes = probes;
    MyF = new FunctionF *[K];
    for (int i = 0 ; i < K ; i ++)
        MyF[i] = new FunctionF();
    TrainData = NULL;
    
}

RandomProjection :: ~RandomProjection(){
    //Simple destructor
    for (int i = 0 ; i < K ; i ++)
        delete MyF[i];
    delete []MyF;
}


void RandomProjection ::SetTrainData(uint8_t * p,int Position){
    //Given a train data point (p) we get its projection and store it
    HypercubePoint * NewPoint = new HypercubePoint(p,K,Position);
    
    //After we create a new HyperCube point we need to fill its Representation array
    for (int i = 0 ; i < K ; i ++)
        NewPoint->Insert(MyF[i]->FindValue(p),i);
    
    if( this->TrainData == NULL){
        //This is our first point, update LastInsert
        TrainData = NewPoint;
        LastInsert = TrainData;
    }
    
    //Not our first, link it to last point and update last
    
    LastInsert->Next=NewPoint;
    LastInsert = LastInsert->Next;
    
}

void RandomProjection ::Train(void){
    int limit = GetTrainNumber();
    for (int i = 0;i<limit;i++){
        SetTrainData(GetRepresenation(i),i);
    }
}

double* RandomProjection :: NearestNeighbour(uint8_t * Query,int Hamming){
    //We project the query to the hypercube dimesnion and get its representation
    //Then the implementation is simillar to the LSH but now we use hamming distance
    HypercubePoint * QueryPoint = new HypercubePoint(Query,K,0);
    for (int i = 0 ; i < K ; i ++)  
        QueryPoint->Insert(MyF[i]->FindValue(Query),i);
    HypercubePoint * temp = TrainData;
    while (temp != NULL)
    {
        if (!GetChecked(temp->GetPosition()) && HammingDistance(QueryPoint->GetRepresentation(),temp->GetRepresentation()) == Hamming){
            double e = Euclidean(Query,temp->GetPixels(),DIMENSION);
            double * ToReturn = new double[2];
            ToReturn[0] = e;
            ToReturn[1]=(double)temp->GetPosition();
            return ToReturn;
        }
        temp = temp->Next;
    }
    double * ToReturn = new double[2];
    ToReturn[0] = -1;
    ToReturn[1]=(double)-1;
    return ToReturn;
    
}


vector <double> RandomProjection :: KNN(int K,uint8_t * Query){
    vector <double> ToReturn;
    int i = 0;
    for (int CurrentHamming = 0; CurrentHamming <= Probes;CurrentHamming++){
        //We keep searching until we reach probes or we find M canditates
        double * Result;
        do
        {
            if (i == this->M ){
                //We need to stop, CurrentHamming = Probes +1 makes the outer loop stop
                CurrentHamming = Probes +1;
                break;
            }
            
            Result = NearestNeighbour(Query,CurrentHamming);
            if (Result[1] != -1){
                SetChecked(Result[1]);
                ToReturn.push_back(Result[0]);
                ToReturn.push_back(Result[1]);
                i++;
            }
        } while (Result[1] != -1);  //If result == -1, no neighbor was found for this hamming, we go to the next one
    }
    bool swapped;
    int j;
    for (i = 0; i < (int)ToReturn.size () - 1; i+=2) {
        swapped = false;
        for (j = 0; j < (int)ToReturn.size () - i - 2; j+=2) {
            if (ToReturn[j+2] == 0  || ToReturn[j+2] == -1 ){
                ToReturn.erase(ToReturn.begin() + j + 2, ToReturn.begin() + j + 4);
                j-=2;
            } 
                
            if (ToReturn[j] > ToReturn[j + 2]) {
                swap(ToReturn[j], ToReturn[j + 2]);
                swap(ToReturn[j+1], ToReturn[j + 3]);
                swapped = true;
            }
        }
 
        // If no two elements were swapped
        // by inner loop, then break
        if (swapped == false)
            break;
        
    }
    for ( j = 1 ; j <(int) ToReturn.size() ; j+=2)
        SetUnchecked(ToReturn[j]);
    return ToReturn;
}


double * RandomProjection :: AccurateNearestNeighbour(uint8_t * Query){
    //Same as above but now we dont need hamming, we check all
    int  MinPos = -1;
    double MinSize = (double)MAXSIZE; 
    HypercubePoint * temp = TrainData;
    while (temp != NULL)
    {
        if (!GetChecked(temp->GetPosition()) ){
            double e = Euclidean(Query,temp->GetPixels(),DIMENSION);
            if (e < MinSize){
                MinSize = e;
                MinPos = temp->GetPosition();
            }
        }
        temp = temp->Next;
    }
    double * ToReturn = new double[2];
    ToReturn[0] = MinSize;
    ToReturn[1]=(double)MinPos;
    return ToReturn;
}

vector <double> RandomProjection :: AccurateKNN(int K,uint8_t * Query){
    vector <double> ToReturn;
    int i = 0;
    double * Result;
    do
    {
        //Loop untill we get K neighbors or no neighbor exists Result[1] will be -1
        if (i == K)
            break;
        Result = AccurateNearestNeighbour(Query);
        if (Result[1] != -1){
                SetChecked(Result[1]);
                ToReturn.push_back(Result[0]);
                ToReturn.push_back(Result[1]);
                i++;
        }
    } while (Result[1] != -1);  
    for (int j = 1 ; j <(int) ToReturn.size() ; j+=2)
        SetUnchecked(ToReturn[j]);
    
    return ToReturn;
}

vector <int> RandomProjection :: RangeSearch(double R,uint8_t * Query){
    //Same as KNN but we check range
    vector <int> ToReturn;
    HypercubePoint * QueryPoint = new HypercubePoint(Query,K,0);
    for (int i = 0 ; i < K ; i ++)  
        QueryPoint->Insert(MyF[i]->FindValue(Query),i);
    HypercubePoint * temp ;
    for ( int Hamming = 0 ; Hamming <= Probes; Hamming ++){
        temp = TrainData;
        while (temp != NULL)
        {
            if (HammingDistance(QueryPoint->GetRepresentation(),temp->GetRepresentation()) == Hamming){
                double e = Euclidean(Query,temp->GetPixels(),DIMENSION);
                if (e <= R)
                    ToReturn.push_back(temp->GetPosition());
            }
            temp=temp->Next;
        }
    }
    return ToReturn;
}
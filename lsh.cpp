#include "lsh.h"
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include "ReadTrainData.h"
#include "hFunc.h"
#include <fstream>


//Define some hyperparameters

#define TABLE_SIZE 5
#define M 100        // Upper bound for M
#define MAXSIZE 1215752202
#define DIMENSION 784
#define WINDOW 5
#define ERROR -1




class Bucket{
    //Class for the buckets of the hash table
    //Each bucket holds only one point
    //The data structure is simillar to linked list
    //So after getting to one cell from the hash table, you get access to the first bucket
    //Then using the NextBucket pointer you can access all buckets-point of the cell
    //This class implement data hiding
    //Each bucket stores the Pixels-bytes representation of the image (Data), ID 
    //and point position to origianl dataset
    private :
        uint8_t * Data;
        int ID,position;
        Bucket * NextBucket;
    public :
        Bucket(){
            Data = NULL;
            NextBucket = NULL;
            ID = -1;
        }
        ~Bucket(){
            if (NextBucket != NULL)
                delete NextBucket;
        }
        void Insert(uint8_t * NewData,int NewID,int NewPosition){
            //Function that inserts a new point to the correct bucket
            //We need to reach the end of the linked list, create a new bucket and link it to the prev bucket
            if (NextBucket == NULL)    //Reached the end
                NextBucket = new Bucket();
            if (ID == -1){      //By default new buckets have ID = -1, so if ID ==-1 this is the bucket to insert to
                
                //The reason we dont just use a constructor with data,id,pos parametres, is because the first bucket
                //will be created from hash table's constructor and will be empty, so the first point will go to that bucket
                //So we need a way to identify the empty buckets
                position = NewPosition;
                Data = NewData;
                ID = NewID;
                return;
            }
            NextBucket->Insert(NewData,NewID,NewPosition);
        }
        double * InnerNearestNeighbour(uint8_t * Query,int QueryId,double * ToReturn){
            //Iteretates buckets and returns ToReturn argument
            if (NextBucket == NULL)
                return ToReturn;
            //The Getchecked function is a fucntion of StoreTrainData, 
            //it indicates whether data point is already included in kNN, if so we dont check it again
            if (ID == QueryId && !GetChecked(position)){
                //Simple implementation of slides
                double e = Euclidean(Query,Data,DIMENSION);
                if (e < ToReturn[0]){
                    ToReturn[0] = e;
                    ToReturn[1] = (double)position;
                }
            }
            if (NextBucket == NULL)     //Reached last bucket
                return ToReturn;
            return NextBucket->InnerNearestNeighbour(Query,QueryId,ToReturn);
        }
        double * InnerNearestNeighbour(uint8_t * Query,int QueryId,double * ToReturn,vector <double> * CurrentVector,int chekced){
            //Iteretates buckets and returns ToReturn argument
            if (NextBucket == NULL)
                return ToReturn;
            //The Getchecked function is a fucntion of StoreTrainData, 
            //it indicates whether data point is already included in kNN, if so we dont check it again
            if (ID == QueryId ){
                //Simple implementation of slides
                bool flag = true;
                for (int i =1 ; i < (int)CurrentVector->size() ; i ++){
                    if ((int) (*CurrentVector)[i] == position){
                        flag = false;
                        break;
                    }
                }
                if (position == chekced)
                    flag = false;
                if (flag){
                    double e = Euclidean(Query,Data,DIMENSION);
                    if (e < ToReturn[0]){
                        ToReturn[0] = e;
                        ToReturn[1] = (double)position;
                    }
                }
            }
            if (NextBucket == NULL)     //Reached last bucket
                return ToReturn;
            return NextBucket->InnerNearestNeighbour(Query,QueryId,ToReturn,CurrentVector,chekced);
        }

        double * NearestNeighbour(uint8_t * Query, int QueryId){
            //Returns a an array of two elements, array[0] is the Disntance to the nearest neighbor
            //array[1] is the position to the dataset of the nearest neighbor
            //This function is used only from the first bucket of the cell
            //If next bucket exists, it calls the InnerNearestNeighbour that iterates the remaining buckets of the cell
            if (Data == NULL)
                return NULL;
            double * ToReturn = new double[2];
            ToReturn[0] = MAXSIZE;
            ToReturn[1] = -1;
            if (ID == QueryId && !GetChecked(position)){
                ToReturn[0] = Euclidean(Query,Data,DIMENSION);
                ToReturn[1] = (double)position;
            }
            if (NextBucket == NULL)
                return ToReturn;
            //Passes pointer to the array that is going to be returned after iteration of all buckets
            return NextBucket->InnerNearestNeighbour(Query,QueryId,ToReturn);
        }
        double * NearestNeighbour(uint8_t * Query, int QueryId,vector <double> * CurrentVector,int chekced){
            //Returns a an array of two elements, array[0] is the Disntance to the nearest neighbor
            //array[1] is the position to the dataset of the nearest neighbor
            //This function is used only from the first bucket of the cell
            //If next bucket exists, it calls the InnerNearestNeighbour that iterates the remaining buckets of the cell
            if (Data == NULL)
                return NULL;
            double * ToReturn = new double[2];
            ToReturn[0] = MAXSIZE;
            ToReturn[1] = -1;
            if (ID == QueryId){
                bool flag = true;
                for (int i =1 ; i < (int)CurrentVector->size() ; i ++){
                    if ((int) (*CurrentVector)[i] == position){
                        flag = false;
                        break;
                    }
                }
                if (position == chekced)
                    flag = false;
                if (flag){
                    ToReturn[0] = Euclidean(Query,Data,DIMENSION);
                    ToReturn[1] = (double)position;
                }
            }
            if (NextBucket == NULL)
                return ToReturn;
            //Passes pointer to the array that is going to be returned after iteration of all buckets
            return NextBucket->InnerNearestNeighbour(Query,QueryId,ToReturn,CurrentVector,chekced);
        }

        double * AccurateInnerNearestNeighbour(uint8_t * Query,double * ToReturn){
            //Same logic as aboce but checks all buckets
            if (NextBucket == NULL)
                return ToReturn;
            if (!GetChecked(position)){
                double e = Euclidean(Query,Data,DIMENSION);
                if (e < ToReturn[0]){
                    ToReturn[0] = e;
                    ToReturn[1] = (double)position;
                }
            }
            if (NextBucket == NULL)
                return ToReturn;
            return NextBucket->AccurateInnerNearestNeighbour(Query,ToReturn);
        }

        double * AccurateNearestNeighbour(uint8_t * Query){
            if (Data == NULL)
                return NULL;
            double * ToReturn = new double[2];
                ToReturn[0] = Euclidean(Query,Data,DIMENSION);
                ToReturn[1] = (double)position;
            if (NextBucket == NULL)
                return ToReturn;
            return NextBucket->AccurateInnerNearestNeighbour(Query,ToReturn);
        }

        vector <int> RangeSearch(double R,uint8_t * Query,int QueryId){
            //Same logic as above, we now return a vector of positions and we check radius
            vector <int> ToReturn;
            if (ID == QueryId && !GetChecked(position)){
                double e = Euclidean(Query,Data,DIMENSION);
                if (e <=R)
                    ToReturn.push_back(position);
            }
            if (NextBucket == NULL)
                return ToReturn;
            return NextBucket->InnerRangeSearch(R,Query,QueryId,&ToReturn);
        }

        vector <int> InnerRangeSearch(double R, uint8_t * Query,int QueryId,vector <int> * ToReturn){
            if (NextBucket == NULL)
                return *ToReturn;
            if (ID == QueryId){
                double e = Euclidean(Query,Data,DIMENSION);
                if (e <=R)
                    (*ToReturn).push_back(position);
            }
            if (NextBucket == NULL)
                return *ToReturn;
            return NextBucket->InnerRangeSearch(R,Query,QueryId,ToReturn);
        }
        
};

class gFunction{
    //Class for g fuction, it konsists of K h functions, and K r random integers
    private :
        int K;
        hFunction ** HashFunctions;
        int * r;
    
    public :
        gFunction(int K,int dimension,int window) : K(K){
            HashFunctions = new hFunction*[K];
            r = new int[K];
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(1, MAXSIZE);
            for (int i = 0 ; i < K ; i ++){
                //Initialize hash LSH functions and random r's
                HashFunctions[i] = new hFunction(dimension,window);
                r[i] = distrib(gen);
            }
        }
        int FindPosition(uint8_t * p ){
            //This is in fact a function that returns the ID of a point, using the formula of the slides
            int temp,temp2,accumulator=0;
            for (int i = 0 ; i < K ; i++){
                temp = r[i] % M;
                temp2 = HashFunctions[i]->operator()(p) %M;
                long long int ToAdd = (long long int)temp * (long long int)temp2;
                ToAdd %=M;

                accumulator += ToAdd;
            }
            return accumulator % M;
        }
        ~gFunction(){
            for (int i = 0 ; i < K;i++){
                delete HashFunctions[i];
            }
            delete [] HashFunctions;
            delete []r;
        }

};

class HashTable {
    //Class for hash table, each hash table consists of table size buckets and a g function
    private : 
        Bucket ** HashBuckets;
        int TableSize;
        gFunction * MyG;
    public :
        HashTable(int K,int dimension,int window,int Size){
            TableSize = Size;
            HashBuckets = new Bucket *[TableSize];

            for (int i = 0 ; i < TableSize; i ++)
                HashBuckets[i] = new Bucket();
            MyG = new gFunction(K,dimension,window);
        }
        void Insert(uint8_t * NewData,int position){
            //Calculate Data's id, get modulo table size and insert to the correspodning bucket
            
            int ID = MyG->FindPosition(NewData);
            HashBuckets[ID%TABLE_SIZE]->Insert(NewData,ID,position);
            
        }
        ~HashTable(){
            for(int i =0;i<TableSize;i++)
                delete HashBuckets[i];
            delete [] HashBuckets;
            delete MyG;
        }
        double *NearestNeighbour(uint8_t * Query){
            int ID = MyG->FindPosition(Query);
            //Returns aproximate nearest neighbor that is returned by nearest neighbor function of the correct bucket
            return (HashBuckets[ID%TableSize]->NearestNeighbour(Query,ID));
        }
        double *NearestNeighbour(uint8_t * Query,vector <double> * CurrentVector,int chekced){
            int ID;
            
        
            ID = MyG->FindPosition(Query);

            //Returns aproximate nearest neighbor that is returned by nearest neighbor function of the correct bucket
            double * ToReturn;

            
            ToReturn = HashBuckets[ID%TableSize]->NearestNeighbour(Query,ID,CurrentVector,chekced);
            
            return (ToReturn);
        }
        
        double *AccurateNearestNeighbour(uint8_t * Query){
            //Returns accurate nearet neighbor, to do so we get the nearest neighbor of all buckets at hash table
            //and then we choose the one with the smaller distance which is the first element of the double array 
            //that is being returned by AccurateNearestNeighbour function of bucket class
            double *ToReturn,*temp;
            for (int i = 0 ; i < TableSize;i++){
                if (i == 0 )
                    ToReturn = HashBuckets[i]->AccurateNearestNeighbour(Query);
                else{
                    
                    temp = HashBuckets[i]->AccurateNearestNeighbour(Query);
                    if (temp  == NULL){
                        continue;
                    }
                    if (temp[0] < ToReturn[0]){
                        delete []ToReturn;
                        ToReturn = temp;
                    }
                    else{
                        delete []temp;
                    }
                }
            }
            return ToReturn;
        }

        vector <int> RangeSearch(double R, uint8_t * Query){
            int ID = MyG->FindPosition(Query);
            //Same as above but for range search
            return HashBuckets[ID%TableSize]->RangeSearch(R,Query,ID);
        }

};


//LSH function implementation follow

LSH :: LSH(int K, int L) : L(L){
    //Simple contructor
    if ( K < 0  || L <0 ){
        cout << "K and L in LSH have to be positive integers\n";
        exit(ERROR);
    }
    MyHash = new HashTable * [L];
    for (int i = 0 ; i < L ; i ++)
        MyHash[i] = new HashTable(K,DIMENSION,WINDOW,TABLE_SIZE);
}

LSH :: ~LSH(){
    //Simple destructor
    for (int i = 0 ; i < L ; i++)
        delete MyHash[i];
    delete [] MyHash;
}


void LSH ::SetTrainData(uint8_t * TrainData,int Position){
    //Given a train data point and its position to dataset insert it to all hash tables
    for (int j = 0 ; j < L ; j++){
        MyHash[j]->Insert(TrainData,Position);
    }
}

double * LSH ::NearestNeighbour(uint8_t * Query){
    //Returns distance (DoubleArrayReturned[0]) and position (DoubleArrayReturned[1]) of the nearest neighbor
    //Note that already checked neighbors are not taken into account
    int  MinPos = -1;
    double MinSize = (double)MAXSIZE,  *ptr; 
    for (int i = 0 ; i < L; i++){
        //Get the NearestNeghbor from all hash table using the correspondin function and store the one with the smallest dist
        ptr = MyHash[i]->NearestNeighbour(Query);
        if (ptr == NULL)
            continue;
        if (ptr[0] < MinSize){
            MinSize = ptr[0];
            MinPos = (int)ptr[1];
        }
        delete []ptr;
    }
    double * ToReturn = new double[2];
    ToReturn[0]=MinSize;
    ToReturn[1]=MinPos;
    return ToReturn; 
}

double * LSH ::NearestNeighbour(uint8_t * Query,vector <double> * CurrentVector,int chekced){
    //Returns distance (DoubleArrayReturned[0]) and position (DoubleArrayReturned[1]) of the nearest neighbor
    //Note that already checked neighbors are not taken into account
    int  MinPos = -1;
    double MinSize = (double)MAXSIZE,  *ptr; 
    for (int i = 0 ; i < L; i++){
        //Get the NearestNeghbor from all hash table using the correspondin function and store the one with the smallest dist
            ptr = MyHash[i]->NearestNeighbour(Query,CurrentVector,chekced);
        
        if (ptr == NULL)
            continue;
        if (ptr[0] < MinSize){
            MinSize = ptr[0];
            MinPos = (int)ptr[1];
        }
        delete []ptr;
    }
    double * ToReturn = new double[2];
    ToReturn[0]=MinSize;
    ToReturn[1]=MinPos;
    return ToReturn; 
}

vector <double>  LSH ::KNN(int K,uint8_t * Query){
    vector <double> ToReturn;
    double * Result;
    for (int i = 0 ; i < K ;i++){
        //Get K nearest neighbors by calling NearestNeighbor K times and each time set as checked the point that was returned
        Result = NearestNeighbour(Query);
        if (Result[1] != -1)
            SetChecked((int)Result[1]);
        ToReturn.push_back(Result[0]);
        ToReturn.push_back(Result[1]);
        delete []Result;
    }
    
    for (int i = 1 ; i < (int)ToReturn.size() ;i +=2){
        if ((int)ToReturn[i] != -1)
            SetUnchecked((int)ToReturn[i]);
    }
    
    return ToReturn;
}

vector <double>  LSH ::KNN(int K,uint8_t * Query,int cheked){
    vector <double> ToReturn;
    double * Result;
    
    for (int i = 0 ; i < K ;i++){
        //Get K nearest neighbors by calling NearestNeighbor K times and each time set as checked the point that was returned
       
        Result = NearestNeighbour(Query,&ToReturn,cheked);
        ToReturn.push_back(Result[0]);
        ToReturn.push_back(Result[1]);
        delete []Result;
    }
    
    return ToReturn;
}

double * LSH ::AccurateNearestNeighbour(uint8_t * Query){
    //Same implementation as aboce but now using the AKNN of hash table
    int  MinPos = -1;
    double MinSize = (double)MAXSIZE,  *ptr;
    
    for (int i = 0 ; i < L; i++){
        ptr = MyHash[i]->AccurateNearestNeighbour(Query);
        if (ptr == NULL)
            continue;
        if (ptr[0] < MinSize){
            MinSize = ptr[0];
            MinPos = (int)ptr[1];
        }
        delete [] ptr;
    }
    
    double * ToReturn = new double[2];
    ToReturn[0]=MinSize;
    ToReturn[1]=MinPos;
    
    return ToReturn; 
}
vector <double>  LSH ::AccurateKNN(int K,uint8_t * Query){
    vector <double> ToReturn ;
    double * Result;
    for (int i = 0 ; i < K ;i++){
        Result = AccurateNearestNeighbour(Query);
        if (Result[1] != -1)
            SetChecked((int)Result[1]);
        ToReturn.push_back(Result[0]);
        ToReturn.push_back(Result[1]);
        delete []Result;
    }
    for (int i = 1 ; i <(int) ToReturn.size() ;i +=2)
        SetUnchecked(ToReturn[i]);
    return ToReturn;
}


void LSH :: Train(void){
    //LSH train itself using the data that are stored to StoreTrainData
    //to do so, it uses GetRepresentation fucntio that returns the pixels-bytes of the i-th image-point of the dataset
    int limit = GetTrainNumber();
    for (int i = 0 ; i < limit ; i++){
        SetTrainData(GetRepresenation(i),i);
    }
}

vector <int> LSH ::RangeSearch(double R,uint8_t * Query){
    //Same logic as aboce but for range search
    vector <int> ToReturn;
    vector <int>temp;
    for (int i = 0 ; i < L ; i++){
        temp = MyHash[i]->RangeSearch(R,Query);
        if (temp.size() == 0)
            continue;
        for (int j = 0 ; j < (int)temp.size() ; j++)
            SetChecked(temp[j]);
        ToReturn.insert( ToReturn.end(), temp.begin(), temp.end() );    
    }
    for (int j = 0 ; j < (int)ToReturn.size() ; j++)
            SetUnchecked(ToReturn[j]);
    return ToReturn;
}
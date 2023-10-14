#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>
#include <cmath>
#include "lsh.h"
double Euclidean2(uint8_t * x, uint8_t * y, int D){
    double ToReturn = 0;
    for (int i = 0 ; i < D;i++)
        ToReturn += pow(x[i] - y[i] , 2);
    
    return sqrt(ToReturn);
}
int main(void){
    /*string s;
    cout << "Please insert dataset path : \n";
    cin >> s;
    ReadTrainData(s);
    //DisplayTrainData();
    cout << "Please insert query set path\n";
    cin >> s;
    ReadQueryData(s);
    DisplayQueryData();*/

    ReadTrainData("dataset.dat");
    ReadQueryData("query.dat");

    LSH * MyLsh = new LSH(10,5);
    MyLsh->Train();
    
    int limit = 5;
    int K = 30,*Result;
    
    for (int i = 0 ; i < limit ; i++){
        cout << "Query :\n";
        DisplayQueryData(i);
        cout << "Result\n";
        Result = MyLsh->KNN(K,GetQueryRepresentation(i));
        MyLsh->AccurateKNN(K,GetQueryRepresentation(i));
        for (int j = 0 ; j < K ; j++)
            DisplayTrainData(Result[j]);
        //DisplayTrainData(MyLsh->NearestNeighbour(GetQueryRepresentation(i)));
        vector <int> Range = MyLsh->RangeSearch(1600,GetQueryRepresentation(i));
        cout << "R-near neighbors:\n";
        for (int j = 0 ; j < (int)Range.size();j++)
            cout << Range[j] << "\n";
    }

    

    cout << Euclidean2(GetQueryRepresentation(0),GetRepresenation(8476),784);
    


    return 0;
}




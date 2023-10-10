#include "ReadTrainData.h"
#include "ReadQueryData.h"
#include <iostream>


int main(void){
    string s;
    cout << "Please insert dataset path : \n";
    cin >> s;
    ReadTrainData(s);
    //DisplayTrainData();
    cout << "Please insert query set path\n";
    cin >> s;
    ReadQueryData(s);
    DisplayQueryData();
    return 0;
}




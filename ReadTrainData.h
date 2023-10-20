
#ifndef DATA_TRAIN

    #define DATA_TRAIN
    #include <string>
    using namespace std;
    int ReadTrainData(string path);
    // void DisplayTrainData(int Pos);
    int GetTrainNumber(void);
    uint8_t * GetRepresenation(int Position);
    void SetChecked(int Position);
    void SetUnchecked(int Position);
    bool GetChecked(int Position);
    void SetCluster(int Position,int NewCluster);
    int GetCluster(int Position);
#endif
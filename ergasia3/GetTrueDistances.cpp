#include "GetTrueDistances.h"

//A function that given a vector of indexes and a query returns the true distances of the query from the vectors with the given indexes
double * GetTrueDistances(vector <int> Indexes,uint8_t * Query ){
    double * Distances = new double[(int)Indexes.size()];
    uint8_t * Train;
    for (int j = 0;j<Indexes.size();j++){
        Train = GetRepresenation(Indexes[j]);
        Distances[j] = Euclidean(Query,Train,GetDimension());
    }    
    return Distances;
}
#include "hFunc.h"
#include <random> 
//Source file for h function from LSH
//Arguments dimension of each point-vector (784 for images) and window

hFunction ::hFunction(int dimension, int UpperWindow) : dimension_(dimension){

            std::random_device rd;  
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(2, UpperWindow);
            window_ = distrib(gen);
            std::normal_distribution<float> v_distribution(0.0, 1.0);
            std::uniform_real_distribution<float> t_distribution(0.0,static_cast<float>(window_));

            std::random_device rd_v;
            std::random_device rd_t;
            
            std::mt19937 v_generator(rd_v());
            std::mt19937 t_generator(rd_t());

            //Initialized distributions for t and v

            // Generate vector v
            for (int i = 0; i < dimension; ++i) {
                v.push_back(v_distribution(v_generator));
            }


            // Initialize the random number generator for 't'
            
            t = t_distribution(t_generator);
        }

        int hFunction ::operator()(uint8_t * p) {
            //Simple implementation of dot product
            float dot_product = 0.0f;
            for (int i = 0; i < dimension_; ++i) {
                //We also normalize by swuaring the dot product
                //This is because ID needs to be positive to be an index of the hash table
                //We also tried getting the absolute value and adding a big number
                //but experimentally we found out this is the best practice
                dot_product += powf(p[i] * v[i],2);
            }
            return static_cast<int>(std::floor((dot_product + t) / window_));
        }

        hFunction::~hFunction(){}

double Euclidean(uint8_t * x, uint8_t * y, int D){      //Simple euclidean implementation
//Arguments two vectors and their dimension
    double ToReturn = 0;
    for (int i = 0 ; i < D;i++)
        ToReturn += pow(x[i] - y[i] , 2);
    
    return sqrt(ToReturn);
}
#include "hFunc.h"
#include <random>

hFunction ::hFunction(int dimension, int window) : dimension_(dimension), window_(window){


            std::normal_distribution<float> v_distribution(0.0, 1.0);
            std::uniform_real_distribution<float> t_distribution(0.0,static_cast<float>(window));

            std::random_device rd_v;
            std::random_device rd_t;
            
            std::mt19937 v_generator(rd_v());
            std::mt19937 t_generator(rd_t());

            // Generate vector v
            for (int i = 0; i < dimension; ++i) {
                v.push_back(v_distribution(v_generator));
            }


            // Initialize the random number generator for 't'
            
            t = t_distribution(t_generator);
        }

        int hFunction ::operator()(uint8_t * p) {
            float dot_product = 0.0f;
            for (int i = 0; i < dimension_; ++i) {
                dot_product += p[i] * v[i];
            }
            return static_cast<int>(std::floor((dot_product + t) / window_));
        }

        hFunction::~hFunction(){}
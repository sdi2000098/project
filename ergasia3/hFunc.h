#ifndef HFUNC
    #include <iostream>
    #include <vector>
    using namespace std;
    #include <stdint.h>
    #define HFUNC

//Header for h function from LSH
//Each LSH function needs to store the dimension of the vector, the window
// also a random vector (v) for dot product and a random float t in [0,w];
double Euclidean(uint8_t * x, uint8_t * y, int D);

class hFunction {
    private:
        int dimension_;
        int window_;
        std::vector<float> v;
        float t;
        
    public:
        
        int operator()(uint8_t * p);        //Returns h(p)
        hFunction(int dimension, int window);
        ~hFunction();
    };

#endif
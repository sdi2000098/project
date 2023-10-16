#ifndef HFUNC
    #include <iostream>
    #include <vector>
    using namespace std;
    #include <stdint.h>
    #define HFUNC

class hFunction {
    private:
        int dimension_;
        int window_;
        std::vector<float> v;
        float t;
        
    public:
        int operator()(uint8_t * p);
        hFunction(int dimension, int window);
        ~hFunction();
    };

#endif
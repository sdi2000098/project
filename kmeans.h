#ifndef K_MEANS
    #include <iostream>
    #define K_MEANS


    class KMeans{
        private :
            int K;
            uint8_t ** Centroids;
            void Lloyds(uint8_t ** Centroids, int K);
            void MyLSH(uint8_t ** Centroids,int KCentroids, int KLSH,int L);
            void MyCube(uint8_t ** Centroids, int KCentroids, int KCube,int M,int probes);
        public :
            KMeans(int K_,char * Method, int KLSH,int L,int Kcube, int M, int probes);
            ~KMeans();

    };
    
    
#endif
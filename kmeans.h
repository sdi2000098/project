#ifndef K_MEANS
    #include <iostream>
    #define K_MEANS

    class Cluster;
    class KMeans{
        private :
            int K;
            Cluster ** MyClusters;
            void Initialize(void);
            void Lloyds();
            void RangeSearch(int KLSH,int L,int KCube,int M,int probes,char * Method);
        public :
            KMeans(int K_,char * Method, int KLSH,int L,int Kcube, int M, int probes);
            ~KMeans();

    };
    
    
#endif
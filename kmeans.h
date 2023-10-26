    #ifndef K_MEANS
    #include <iostream>
    #define K_MEANS

    class Cluster;
    class KMeans{
        private :
            int K;
            Cluster ** MyClusters;          //Array of pointers to Clusters
            void Initialize(void);          //Kmeans++ init
            void Lloyds();
            void RangeSearch(int KLSH,int L,int KCube,int M,int probes,char * Method);
            void Silhouette(void);
            double GetMean(int i,int ClusterIndex);
        public :
            KMeans(int K_,char * Method, int KLSH,int L,int Kcube, int M, int probes,const char * outputfileName,bool flag);
            //All arguments are passed to constructor, even if they are not used
            ~KMeans();

    };
    
    
#endif
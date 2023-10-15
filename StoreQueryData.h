#ifndef STORE_QUERY
    #include "StoreTrainData.h"

    #define STORE_QUERY



    class QueryStore{
        public :
            void Insert(Image * NewImage);
            QueryStore();
            ~QueryStore();
            // void Display(int Pos);
            uint8_t * GetRepresentation(int Position);
        private :
            Image ** AllImages;
            int NumberOfImages;
    };


    

#endif
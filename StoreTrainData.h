#ifndef STORE_TRAIN
    #include <stdint.h>
    #define STORE_TRAIN
    int reverseInt (int x);
    class Image{
        public : 
            void Insert(uint8_t NewByte);
            Image();
            ~Image();
            // void Display(void);
            uint8_t * GetRepresenation(void);
            void SetChecked(void);
            void SetUnchecked(void);
            bool GetChecked(void);
            int GetCluster(void);
            void SetCluster(int NewCluster);
        private :
            uint8_t * Pixels;
            int NumberOfPixels;
            bool checked;
            int cluster;
        
    };

    class TrainStore{
        public :
            void Insert(Image * NewImage);
            TrainStore();
            ~TrainStore();
            // void Display(int Pos);
            uint8_t * GetRepresenation(int Position);
            void SetChecked (int Position);
            void SetUnchecked(int Position);
            bool GetChecked(int Position);
            int GetCluster(int Position);
            void SetCluster(int Position,int NewCluster);

        private :
            Image ** AllImages;
            int NumberOfImages;
    };

    

#endif
#ifndef STORE_TRAIN
    #include <stdint.h>
    #define STORE_TRAIN
    int reverseInt (int x);
    class Image{
        public : 
            void Insert(uint8_t NewByte);
            Image();
            ~Image();
            void Display(void);
            uint8_t * GetRepresenation(void);
        private :
            uint8_t * Pixels;
            int NumberOfPixels;
        
    };

    class TrainStore{
        public :
            void Insert(Image * NewImage);
            TrainStore();
            ~TrainStore();
            void Display(int Pos);
            uint8_t * GetRepresenation(int Position);
        private :
            Image ** AllImages;
            int NumberOfImages;
    };

    

#endif
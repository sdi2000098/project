#ifndef STORE_TRAIN
    #include <stdint.h>
    #define STORE_TRAIN

    class Image{
        public : 
            void Insert(uint8_t NewByte);
            Image();
            ~Image();
            void Display(void);
        private :
            uint8_t * Pixels;
            int NumberOfPixels;
        
    };

    class TrainStore{
        public :
            void Insert(Image * NewImage);
            TrainStore();
            ~TrainStore();
            void Display(void);
        private :
            Image ** AllImages;
            int NumberOfImages;
    };

    

#endif
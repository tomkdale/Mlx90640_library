//------------------------------------------
//mlx90640.h code to read data from melexis mlx90640 IR array
//Tom Dale & Matthew Craig
// 1-23-2019
//
//Code supports image mode only, absolute temperature is calculated, only relative temperature.
//------------------------------------------

#ifndef mlx90640_h
#define mlx90640_h

//to use setRefRate pass in one of these values
//default is 16hz
#define mlxRef_0hz5 0x0 
#define mlxRef_2hz 0x1 
#define mlxRef_4hz 0x2 
#define mlxRef_8hz 0x3 
#define mlxRef_16hz 0x4 
#define mlxRef_32hz 0x5 
#define mlxRef_64hz 0x6

#include "mbed.h"

class mlx90640{
     private:
        I2C _i2c;
        uint8_t mlxRead;//addresses to read and write to slave
        uint8_t mlxWrite; 
        void clearReadBit(void);
    public:
        mlx90640(I2C i2c);
        int getImageFrame(uint16_t* image);//sets 768 image values relative temperatures recorded on each IR sensor
        void setRefRate(const int refreshRate);//sets refresh rate, preprocessor directive for variable
        bool isReady(void);//returns true if there is a new unread page
      
   
};

#endif
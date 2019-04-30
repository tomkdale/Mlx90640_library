//------------------------------------------
//mlx90640.cpp code to read data from melexis mlx90640 IR array
//Tom Dale & Matthew Craig
// 1-23-2019
//
//Code supports image mode only, absolute temperature is calculated, only relative temperature.
//------------------------------------------

#include <mbed.h>
#include "mlx90640.h"
#define mlxAddr 0x33
//register addresses are 2 bytes so for easy i2c passing break up preprocessors into most significant and least significant bytes
#define DATA_REG_MS 0x04
#define DATA_REG_LS 0x00
#define CONTROL_REG_MS 0x80
#define CONTROL_REG_LS 0x0D
#define STATUS_REG_MS 0x80
#define STATUS_REG_LS 0x00


mlx90640::mlx90640(I2C i2c): _i2c(i2c){//initialize this mlx object
    _i2c.frequency(400000);//default i2c frequency for mlx
    mlxWrite = mlxAddr << 1;//set mlx address for read and write
    mlxRead = mlxAddr << 1 | 0x01;
    clearReadBit();//start with cleared read bit so device can write fresh data to its RAM
}

int mlx90640::getImageFrame(uint16_t* image){//calculate 768 values and put them into the given array pointer. returns 0 if device reads properly
    _i2c.start();//check if device is connected return -1 if not
    if(!(_i2c.write(mlxWrite)))return -1;
    _i2c.stop();
    char i2cData[1536];//2 bytes for all 768 pixels
    
    char reg[2] = {DATA_REG_MS,DATA_REG_LS};//register location to read from 
    _i2c.write(mlxWrite,reg,2);//writes to data register address
    
    _i2c.read(mlxRead,i2cData,1536);//read from entire page 
    //values here are relative, and only to be used for images, temperature calculations currently not implemented
    int j = 0;//iterator for byte values
    for(int i = 0;i < 767;i = i++){//combine every pair of data into 1 16 bit value
        image[i] =((uint16_t)i2cData[j] << 8) + (i2cData[j+1]&0x00FF);
        j = j+2;
    }
    //now clear read bit
    clearReadBit();
    return 0;
}

void mlx90640::setRefRate(const int refreshRate){//sets refrence rate of MLX
    char buff[2]= {CONTROL_REG_MS,CONTROL_REG_LS};//control register address in 2 byte values
    _i2c.write(mlxWrite,buff,2);//point to control register
    _i2c.read(mlxRead,buff,2);//Reads the control register, a 16 bit register which controls several settings on the sensor 
    //settings register is 4 bytes so calculations use 2 char values
    buff[1] = buff[1] & 0xFC;//clear existing refresh rate data
    buff[0] = buff[0] & 0x7F;
    buff[1] = buff[1] |( refreshRate >> 1);//set the new values of refresh rate without changing data in the Control Register
    buff[0] = buff[0] | (refreshRate << 7);
    _i2c.write(mlxAddr,buff,2);//Writes the new refresh rate data to the control register
}

bool mlx90640::isReady(void){//returns true if there is a new unread page
    char buff[2] = {STATUS_REG_MS,STATUS_REG_LS};
    _i2c.write(mlxWrite,buff,2);
    _i2c.read(mlxRead,buff,1);
    if(buff[0] & 0x8){//if read bit is on then device is ready to be read from
        return true;   
    }
    return false;
    //note, once data is read mlx automatically sets read bit back to 0 until new data is loaded
}


void mlx90640::clearReadBit(void){//clear read bit to allow the mlx to keep writing data to RAM
    char buff[4];
    buff[0] =STATUS_REG_MS;//set register address to write to
    buff[1] =STATUS_REG_LS;
    char status[2];
    _i2c.write(mlxWrite,buff,2);//writes to data register address
    _i2c.read(mlxRead,status,2);//read 2 bytes of status info
    buff[2] = status[0];//keep status bits the same except clear bit3
    buff[3] = status[1] & 0xF7;
    _i2c.write(mlxWrite,buff,4);
    
    
    
    
}

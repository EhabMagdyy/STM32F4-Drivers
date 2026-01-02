#include "interface/HAL/hi2c.h"

STD_ReturnType HI2C_Init(HI2C_t* hi2c){
    STD_ReturnType ret = STD_SUCCESS;
    if(hi2c == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = I2C_Init(hi2c->i2cConfig);

        if(hi2c->i2cConfig->dmaEnable == I2C_DMA_ENABLE){
            ret = DMA_Init(hi2c->dmaInstance);
        }
    }
    return ret;
}
STD_ReturnType HI2C_DeInit(HI2C_t* hi2c){
    STD_ReturnType ret = STD_SUCCESS;
    if(hi2c == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = I2C_DeInit(hi2c->i2cConfig);

        if(hi2c->i2cConfig->dmaEnable == I2C_DMA_ENABLE){
            ret = DMA_DeInit(hi2c->dmaInstance);
        }
    }
    return ret;
}
STD_ReturnType HI2C_Master_Transmit(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer, uint32_t timeoutMS){
    STD_ReturnType ret = STD_SUCCESS;
    if(hi2c == NULL || buffer == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = I2C_Master_Transmit(hi2c->i2cConfig, devAddress, buffer, 1000);
    }
    return ret;
}
STD_ReturnType HI2C_Master_Receive(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer, uint32_t timeoutMS){
    STD_ReturnType ret = STD_SUCCESS;
    if(hi2c == NULL || buffer == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = I2C_Master_Receive(hi2c->i2cConfig, devAddress, buffer, 1000);
    }
    return ret;
}

STD_ReturnType HI2C_Master_TransmitIT(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer){
    STD_ReturnType ret = STD_SUCCESS;
    if(hi2c == NULL || buffer == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = I2C_Master_TransmitIT(hi2c->i2cConfig, devAddress, buffer);
    }
    return ret;
}

STD_ReturnType HI2C_Master_ReceiveIT(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer){
    STD_ReturnType ret = STD_SUCCESS;
    if(hi2c == NULL || buffer == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = I2C_Master_ReceiveIT(hi2c->i2cConfig, devAddress, buffer);
    }
    return ret;
}

STD_ReturnType HI2C_Master_TransmitDMA(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer){
    STD_ReturnType ret = STD_SUCCESS;
    if(hi2c == NULL || buffer == NULL){
        ret = STD_ERROR;
    }
    else{
        
    }
    return ret;
}

STD_ReturnType HI2C_Master_ReceiveDMA(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer){
    STD_ReturnType ret = STD_SUCCESS;
    if(hi2c == NULL || buffer == NULL){
        ret = STD_ERROR;
    }
    else{
        
    }
    return ret;
}
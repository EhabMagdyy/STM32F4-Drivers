#include "interface/HAL/eeprom.h"
#include "interface/Core/systick.h"

uint8_t i2cData[2];
I2C_Buffer_t i2cBuffer;

STD_ReturnType EEPROM_Init(const EEPROM_Config_t* config){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = I2C_Init(config->i2cConfig);
    }
    return ret;
}

STD_ReturnType EEPROM_DeInit(const EEPROM_Config_t* config){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = I2C_DeInit(config->i2cConfig);
    }
    return ret;
}

STD_ReturnType EEPROM_ReadData(const EEPROM_Config_t* config, uint16_t address, uint8_t* data){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL || data == NULL){
        ret = STD_ERROR;
    }
    else{
        *data = address & 0xFF;
        i2cBuffer.data = data;
        i2cBuffer.length = 1;
        ret = I2C_Master_TransmitIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cBuffer);
        SYSTICK_DelayMS(10);
        ret = I2C_Master_ReceiveIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cBuffer);
    }
    return ret;
}

STD_ReturnType EEPROM_WriteData(const EEPROM_Config_t* config, uint16_t address, uint8_t data){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL){
        ret = STD_ERROR;
    }
    else{
        i2cData[0] = address & 0xFF;
        i2cData[1] = data;
        i2cBuffer.data = i2cData;
        i2cBuffer.length = 2;

        ret = I2C_Master_TransmitIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cBuffer);
    }
    return ret;
}

STD_ReturnType EEPROM_WritePage(const EEPROM_Config_t* config, uint16_t address, const I2C_Buffer_t* buffer){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL || buffer == NULL || buffer->data == NULL || buffer->length == 0){
        ret = STD_ERROR;
    }
    else{
        
    }
    return ret;
}

STD_ReturnType EEPROM_ReadPage(const EEPROM_Config_t* config, uint16_t address, const I2C_Buffer_t* buffer){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL || buffer == NULL || buffer->data == NULL || buffer->length == 0){
        ret = STD_ERROR;
    }
    else{
        
    }
    return ret;
}
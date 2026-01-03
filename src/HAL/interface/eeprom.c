#include "interface/HAL/eeprom.h"
#include "interface/Core/systick.h"
#include <string.h>

uint8_t i2cData[17];    // 1 byte for address + 16 bytes for data
I2C_Buffer_t i2cBuffer; // Buffer for I2C operations
I2C_Buffer_t i2cAddr; // Buffer for I2C operations
uint8_t addr;


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

STD_ReturnType EEPROM_WriteData(const EEPROM_Config_t* config, uint16_t address, uint8_t* data, uint8_t length){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL || data == NULL || length == 0 || length > 16){
        ret = STD_ERROR;
    }
    else{
        i2cData[0] = address & 0xFF;    // Set the memory address to write to
        memcpy(i2cData+1, data, length);
        // Prepare I2C buffer
        i2cBuffer.data = i2cData;
        i2cBuffer.length = length + 1;
        // Transmit data to EEPROM
        ret = I2C_Master_TransmitIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cBuffer);
    }
    return ret;
}

STD_ReturnType EEPROM_ReadData(const EEPROM_Config_t* config, uint16_t address, uint8_t* data, uint8_t length){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL || data == NULL || length == 0 || length > 16){
        ret = STD_ERROR;
    }
    else{
        addr = address & 0xFF; // Set the memory address to read from
        // Prepare I2C buffer
        i2cAddr.data = &addr;
        i2cAddr.length = 1;
        // Send memory address
        ret = I2C_Master_TransmitIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cAddr);
        SYSTICK_DelayMS(10);
        i2cBuffer.length = length;
        i2cBuffer.data = data;
        // Read data from EEPROM
        ret = I2C_Master_ReceiveIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cBuffer);
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
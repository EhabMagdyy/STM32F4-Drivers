#include "interface/HAL/eeprom.h"
#include "OS/scheduler.h"
#include <string.h>

uint8_t i2cData[17];    // 1 byte for address + 16 bytes for data
I2C_Buffer_t i2cBuffer; // Buffer for I2C operations
I2C_Buffer_t i2cAddr; // Buffer for I2C operations
uint8_t blockAddress;

volatile EEPROM_State_t eepromState = EEPROM_IDLE;

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
    else if(I2C_GetState(config->i2cConfig->i2cNumber) != I2C_STATE_IDLE){
        ret = STD_BUSY;
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
    else if(I2C_GetState(config->i2cConfig->i2cNumber) != I2C_STATE_IDLE){
        ret = STD_BUSY;
    }
    else{
        if(eepromState == EEPROM_IDLE){
            blockAddress = address & 0xFF; // Set the memory address to read from
            // Prepare I2C buffer
            i2cAddr.data = &blockAddress;
            i2cAddr.length = 1;
            i2cBuffer.length = length;
            i2cBuffer.data = data;
            // Send memory address
            ret = I2C_Master_TransmitIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cAddr);
            eepromState = EEPROM_READ_DATA;
        }
        else if(eepromState == EEPROM_READ_DATA){
            ret = I2C_Master_ReceiveIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cBuffer);
            eepromState = EEPROM_IDLE;
        }
        else{
            ret = STD_BUSY;
        }
    }
    return ret;
}

STD_ReturnType EEPROM_WritePage(const EEPROM_Config_t* config, uint8_t addressOffset, uint8_t* data){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL || data == NULL){
        ret = STD_ERROR;
    }
    else{
        EEPROM_WriteData(config, addressOffset*16, data, 16);
    }
    return ret;
}

STD_ReturnType EEPROM_ReadPage(const EEPROM_Config_t* config, uint8_t addressOffset, uint8_t* data){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL || data == NULL){
        ret = STD_ERROR;
    }
    else{
        EEPROM_ReadData(config, addressOffset*16, data, 16);
    }
    return ret;
}

STD_ReturnType EEPROM_WriteVerify(const EEPROM_Config_t* config, uint8_t address, uint8_t* txData, uint8_t* rxData, uint8_t length){
    STD_ReturnType ret = STD_SUCCESS;
    if(config == NULL || txData == NULL || rxData == NULL){
        ret = STD_ERROR;
    }
    else{
        if(eepromState == EEPROM_IDLE){
            i2cData[0] = address & 0xFF;    // Set the memory address to write to
            memcpy(i2cData+1, txData, length);
            // Prepare I2C buffer
            i2cBuffer.data = i2cData;
            i2cBuffer.length = length + 1;
            // Transmit data to EEPROM
            ret = I2C_Master_TransmitIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cBuffer);
            eepromState = EEPROM_READ_SEND_ADDR;
        }
        else if(eepromState == EEPROM_READ_SEND_ADDR){
            blockAddress = address & 0xFF; // Set the memory address to read from
            // Prepare I2C buffer
            i2cAddr.data = &blockAddress;
            i2cAddr.length = 1;
            // Send memory address
            ret = I2C_Master_TransmitIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cAddr);
            eepromState = EEPROM_READ_DATA;
        }
        else if(eepromState == EEPROM_READ_DATA){
            i2cBuffer.length = length;
            i2cBuffer.data = rxData;
            // Receive data from EEPROM
            ret = I2C_Master_ReceiveIT(config->i2cConfig, config->deviceAddress | (address >> 8), &i2cBuffer);
            eepromState = EEPROM_IDLE;
        }
        else{
            ret = STD_BUSY;
        }
    }
    return ret;
}
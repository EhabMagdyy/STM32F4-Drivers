#ifndef EEPROM_H
#define EEPROM_H

#include "interface/MCAL/i2c.h"

typedef void (*EEPROM_Callback_t)(uint8_t);

typedef struct{
    I2C_Config_t* i2cConfig;
    uint8_t deviceAddress;
    EEPROM_Callback_t operationCompleteCallback;
} EEPROM_Config_t;

typedef enum{
    EEPROM_READ_SEND_ADDR = 0,
    EEPROM_READ_DATA,
    EEPROM_IDLE
} EEPROM_State_t;

STD_ReturnType EEPROM_Init(const EEPROM_Config_t* config);
STD_ReturnType EEPROM_DeInit(const EEPROM_Config_t* config);
STD_ReturnType EEPROM_WriteData(const EEPROM_Config_t* config, uint16_t address, uint8_t* data, uint8_t length);
STD_ReturnType EEPROM_ReadData(const EEPROM_Config_t* config, uint16_t address, uint8_t* data, uint8_t length);
STD_ReturnType EEPROM_WritePage(const EEPROM_Config_t* config, uint8_t pageOffset, uint8_t* buffer);
STD_ReturnType EEPROM_ReadPage(const EEPROM_Config_t* config, uint8_t pageOffset, uint8_t* buffer);
STD_ReturnType EEPROM_WriteVerify(const EEPROM_Config_t* config, uint16_t address, uint8_t* txData, uint8_t* rxData, uint8_t length);

#endif // EEPROM_H
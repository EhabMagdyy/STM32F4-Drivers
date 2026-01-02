#ifndef EEPROM_H
#define EEPROM_H

#include "interface/MCAL/i2c.h"

typedef struct{
    int* data;
    uint8_t length;
} EEPROM_Buffer_t;

typedef struct{
    I2C_Config_t i2cConfig;
    uint8_t deviceAddress;
} EEPROM_Config_t;

STD_ReturnType EEPROM_Init(const EEPROM_Config_t* config);
STD_ReturnType EEPROM_DeInit(const EEPROM_Config_t* config);
STD_ReturnType EEPROM_ReadData(const EEPROM_Config_t* config, uint16_t address, EEPROM_Buffer_t* buffer);
STD_ReturnType EEPROM_WriteData(const EEPROM_Config_t* config, uint16_t address, const EEPROM_Buffer_t* buffer);
STD_ReturnType EEPROM_WritePage(const EEPROM_Config_t* config, uint16_t address, const EEPROM_Buffer_t* buffer);
STD_ReturnType EEPROM_ReadPage(const EEPROM_Config_t* config, uint16_t address, const EEPROM_Buffer_t* buffer);

#endif // EEPROM_H
#include "interface/MCAL/i2c.h"
#include "interface/MCAL/dma.h"

typedef struct{
    I2C_Config_t* i2cConfig;
    DMA_Instance_t* dmaInstance;
} HI2C_t;

STD_ReturnType HI2C_Init(HI2C_t* hi2c);
STD_ReturnType HI2C_DeInit(HI2C_t* hi2c);
STD_ReturnType HI2C_Master_Transmit(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer, uint32_t timeoutMS);
STD_ReturnType HI2C_Master_Receive(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer, uint32_t timeoutMS);
STD_ReturnType HI2C_Master_TransmitIT(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer);
STD_ReturnType HI2C_Master_ReceiveIT(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer);
STD_ReturnType HI2C_Master_TransmitDMA(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer);
STD_ReturnType HI2C_Master_ReceiveDMA(HI2C_t* hi2c, uint16_t devAddress, I2C_Buffer_t* buffer);
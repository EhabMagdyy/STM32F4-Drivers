#ifndef I2C_H
#define I2C_H

#include "../../../lib/STD_Types.h"

#define I2C_MIN_CLOCK_FREQ 2000000U   // 2 MHz
#define I2C_MAX_CLOCK_FREQ 50000000U  // 50 MHz

#define I2C_DMA_DISABLE 0
#define I2C_DMA_ENABLE  1

typedef enum{
    I2C_STATE_IDLE,
    I2C_STATE_TX_BUSY,
    I2C_STATE_RX_BUSY,
    I2C_STATE_ERROR
} I2C_State_t;

typedef enum{
    I2C_1,
    I2C_2,
    I2C_3
} I2C_Number_t;

typedef enum{
    I2C_MASTER_MODE,
    I2C_SLAVE_MODE
} I2C_Mode_t;

typedef enum{
    I2C_7BIT_ADDR_MODE,
    I2C_10BIT_ADDR_MODE
} I2C_ADDR_MODE_t;

typedef enum{
    I2C_BUS_SPEED_STANDARD, // 100 kHz
    I2C_BUS_SPEED_FAST,     // 400 kHz
} I2C_Bus_Speed_t;

typedef struct{
    uint8_t* data;
    uint16_t length;
    uint16_t index;
} I2C_Buffer_t;

typedef void (*I2C_Callback_t)(void);

typedef struct{
    I2C_Number_t i2cNumber;
    I2C_Mode_t mode;
    uint32_t apb1ClockFreq; // in Hz
    I2C_Bus_Speed_t busSpeed;
    I2C_ADDR_MODE_t addrMode;
    uint16_t ownAddress;  // 7-bit or 10-bit address
    I2C_Callback_t transmitCallback;
    I2C_Callback_t receiveCallback;
    uint8_t dmaEnable;
} I2C_Config_t;

STD_ReturnType I2C_Init(const I2C_Config_t* config);
STD_ReturnType I2C_DeInit(const I2C_Config_t* config);
STD_ReturnType I2C_Master_Transmit(const I2C_Config_t* config, uint16_t devAddress, I2C_Buffer_t* buffer, uint32_t timeoutMS);
STD_ReturnType I2C_Master_Receive(const I2C_Config_t* config, uint16_t devAddress, I2C_Buffer_t* buffer, uint32_t timeoutMS);

STD_ReturnType I2C_Master_TransmitIT(const I2C_Config_t* config, uint16_t devAddress, I2C_Buffer_t* buffer);
STD_ReturnType I2C_Master_ReceiveIT(const I2C_Config_t* config, uint16_t devAddress, I2C_Buffer_t* buffer);

I2C_State_t I2C_GetState(I2C_Number_t i2cNumber);


#endif // I2C_H
#ifndef SPI_H
#define SPI_H

#include "../../../lib/STD_Types.h"

#define SPI_SS_PIN_SET      1U
#define SPI_SS_PIN_RESET    0U

typedef enum{
    SPI_1 = 0,
    SPI_2,
    SPI_3,
    SPI_4       // Not available on STM32F401 Discovery Board
} SPI_Number_t;

typedef enum{
    SPI_DIRECTION_2LINES = 0,
    SPI_DIRECTION_2LINES_RXONLY,
    SPI_DIRECTION_1LINE_RX,
    SPI_DIRECTION_1LINE_TX
} SPI_Direction_t;

typedef enum{
    SPI_DATA_FRAME_8BIT = 0,
    SPI_DATA_FRAME_16BIT
} SPI_DataFrame_t;

typedef enum{
    SPI_MODE_MASTER = 0,
    SPI_MODE_SLAVE
} SPI_Mode_t;

typedef enum{
    SPI_CLOCK_POLARITY_LOW = 0,
    SPI_CLOCK_POLARITY_HIGH
} SPI_ClockPolarity_t;

typedef enum{
    SPI_CLOCK_PHASE_1EDGE = 0,
    SPI_CLOCK_PHASE_2EDGE
} SPI_ClockPhase_t;

typedef enum{
    SPI_FRAME_FORMAT_MSB_FIRST = 0,
    SPI_FRAME_FORMAT_LSB_FIRST
} SPI_FrameFormat_t;

typedef enum{
    SPI_BAUDRATE_PRESCALER_2 = 0,
    SPI_BAUDRATE_PRESCALER_4,
    SPI_BAUDRATE_PRESCALER_8,
    SPI_BAUDRATE_PRESCALER_16,
    SPI_BAUDRATE_PRESCALER_32,
    SPI_BAUDRATE_PRESCALER_64,
    SPI_BAUDRATE_PRESCALER_128,
    SPI_BAUDRATE_PRESCALER_256
} SPI_BaudRatePrescaler_t;

typedef enum{
    SPI_FRAME_FORMAT_MOTOROLA = 0,
    SPI_FRAME_FORMAT_TI
} SPI_FrameFormatStandard_t;

#define SPI_DMA_ENABLE      1U
#define SPI_DMA_DISABLE     0U

typedef void (*SPI_Callback_t)(void);

typedef enum{
    SPI_STATE_READY = 0,
    SPI_STATE_BUSY,
    SPI_STATE_ERROR
} SPI_State_t;

#define SPI_DMA_ENABLE      1U
#define SPI_DMA_DISABLE     0U

typedef struct{
    uint8_t* txData;
    uint8_t* rxData;
    uint8_t length;
    uint8_t txIndex;
    uint8_t rxIndex;
} SPI_Buffer_t;

typedef struct{
    SPI_Number_t spiNum;
    SPI_Mode_t mode;
    SPI_Direction_t direction;
    SPI_DataFrame_t dataFrame;
    SPI_ClockPolarity_t clockPolarity;
    SPI_ClockPhase_t clockPhase;
    SPI_FrameFormat_t frameFormat;
    SPI_BaudRatePrescaler_t baudRatePrescaler;
    SPI_FrameFormatStandard_t frameFormatStandard;
    SPI_Callback_t trancieveCallback;
    uint8_t dmaEnable;
} SPI_Config_t;

STD_ReturnType SPI_Init(const SPI_Config_t* config);
STD_ReturnType SPI_DeInit(const SPI_Config_t* config);
// Transmit & Receive Synchronously
STD_ReturnType SPI_Tranceive(const SPI_Config_t* config, SPI_Buffer_t* buffer, uint32_t timeout);
// Transmit & Receive Using Interrupts
STD_ReturnType SPI_TranceiveIT(const SPI_Config_t* config, SPI_Buffer_t* buffer);
STD_ReturnType SPI_SetSSGPIOPin(SPI_Number_t spiNum, uint8_t pinState);
// SPI Gettters & Setters
STD_ReturnType SPI_GetTXEFlag(SPI_Number_t spiNum, uint8_t* status);
STD_ReturnType SPI_GetRXNEFlag(SPI_Number_t spiNum, uint8_t* status);
STD_ReturnType SPI_GetBusyFlag(SPI_Number_t spiNum, uint8_t* status);
STD_ReturnType SPI_SetState(SPI_Number_t spiNum, uint8_t state);
STD_ReturnType SPI_GetState(SPI_Number_t spiNum, uint8_t* state);
STD_ReturnType SPI_NVIC_IRQ_Enable(SPI_Number_t spiNum);
STD_ReturnType SPI_NVIC_TXE_RXNE_Disable(SPI_Number_t spiNum);
STD_ReturnType SPI_GetDataSize(SPI_Number_t spiNum, uint8_t* dataSize);
STD_ReturnType SPI_GetDR8Bits(SPI_Number_t spiNum, uint8_t* data);
STD_ReturnType SPI_SetDR8Bits(SPI_Number_t spiNum, uint8_t* data);
STD_ReturnType SPI_GetDR16Bits(SPI_Number_t spiNum, uint8_t* data);
STD_ReturnType SPI_SetDR16Bits(SPI_Number_t spiNum, uint8_t* data);
uint32_t SPI_DRAddress(SPI_Number_t spiNum);

#endif // SPI_H
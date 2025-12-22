#ifndef SPI_H
#define SPI_H

#include "../../../lib/STD_Types.h"
#include "interface/Core/systick.h"

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

typedef struct{
    uint8_t* txData;
    uint8_t* rxData;
    uint8_t length;
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
} SPI_Config_t;

STD_ReturnType SPI_Init(const SPI_Config_t* config, SYSTICK_ClockSource_t clockSource);
STD_ReturnType SPI_DeInit(const SPI_Config_t* config);
STD_ReturnType SPI_Transmit(const SPI_Config_t* config, SPI_Buffer_t* buffer, uint32_t timeout);
STD_ReturnType SPI_Tranceive(const SPI_Config_t* config, SPI_Buffer_t* buffer, uint32_t timeout);
#endif // SPI_H
#ifndef H_SERIAL_H
#define H_SERIAL_H

#include "../../../lib/STD_Types.h"
#include "interface/MCAL/uart.h"
#include "interface/MCAL/dma.h"

typedef struct{
    uint8_t* src;
    uint8_t* dest;
    uint32_t length;
} HSerial_Buffer_t;

typedef struct{
    UART_Config_t* uartConfig;
    DMA_Instance_t* dmaConfig;
    HSerial_Buffer_t* buffer;
} HSerial_Config_t;

STD_ReturnType HSerial_Init(HSerial_Config_t* hserialConfig, SYSTICK_ClockSource_t clockSource);
STD_ReturnType HSerial_DeInit(HSerial_Config_t* hserialConfig);
STD_ReturnType HSerial_SendBuffer(HSerial_Config_t* hserialConfig);
// STD_ReturnType HSerial_ReceiveBuffer(HSerial_Config_t* hserialConfig);

#endif // H_SERIAL_H
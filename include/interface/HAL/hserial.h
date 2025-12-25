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

typedef struct {
    UART_Config_t* uartConfig;
    DMA_Instance_t* txDma;
    DMA_Instance_t* rxDma;
    HSerial_Buffer_t* txBuffer;
    HSerial_Buffer_t* rxBuffer;
} HSerial_Config_t;


STD_ReturnType HSerial_Init(HSerial_Config_t* hserialConfig, SYSTICK_ClockSource_t clockSource);
STD_ReturnType HSerial_DeInit(HSerial_Config_t* hserialConfig);
STD_ReturnType HSerial_SendBuffer(HSerial_Config_t* hserialConfig, uint32_t timeoutMS);
STD_ReturnType HSerial_ReceiveBuffer(HSerial_Config_t* hserialConfig, uint32_t timeoutMS);
STD_ReturnType HSerial_SendBufferIT(HSerial_Config_t* hserialConfig);
STD_ReturnType HSerial_ReceiveBufferIT(HSerial_Config_t* hserialConfig);
STD_ReturnType HSerial_SendBufferDMA(HSerial_Config_t* hserialConfig);
STD_ReturnType HSerial_ReceiveBufferDMA(HSerial_Config_t* hserialConfig);
STD_ReturnType HSerial_TxGetStateDMA(HSerial_Config_t* hserialConfig, DMA_State_t* state);
STD_ReturnType HSerial_RxGetStateDMA(HSerial_Config_t* hserialConfig, DMA_State_t* state);

#endif // H_SERIAL_H
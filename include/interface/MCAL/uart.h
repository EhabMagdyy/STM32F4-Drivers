#ifndef UART_H
#define UART_H

#include "../../../lib/STD_Types.h"
#include "interface/Core/systick.h"

typedef struct{
    volatile uint32_t SR;     // 0x00
    volatile uint32_t DR;     // 0x04
    volatile uint32_t BRR;    // 0x08
    volatile uint32_t CR1;    // 0x0C
    volatile uint32_t CR2;    // 0x10
    volatile uint32_t CR3;    // 0x14
    volatile uint32_t GTPR;   // 0x18
} UART_Reg_t;

#define UART1   ((UART_Reg_t*)0x40011000UL)
#define UART2   ((UART_Reg_t*)0x40004400UL)
#define UART6   ((UART_Reg_t*)0x40011400UL)

typedef UART_Reg_t* UART_Instance_t;

typedef enum{   
    UART_BAUDRATE_9600   = 9600,
    UART_BAUDRATE_19200  = 19200,
    UART_BAUDRATE_38400  = 38400,
    UART_BAUDRATE_57600  = 57600,
    UART_BAUDRATE_115200 = 115200,
    UART_BAUDRATE_230400 = 230400,
    UART_BAUDRATE_460800 = 460800,
    UART_BAUDRATE_921600 = 921600,
} UART_BaudRate_t;

typedef enum{
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN = 1,
    UART_PARITY_ODD  = 2
} UART_Parity_t;

typedef enum{
    UART_DATABITS_8  = 8,
    UART_DATABITS_9  = 9
} UART_DataBits_t;

typedef void (*CBFunc_t)(void);

#define UART_INTERRUPT_ENABLE    1
#define UART_INTERRUPT_DISABLE   0

typedef struct{
    UART_Instance_t UartInstance;
    UART_BaudRate_t BaudRate;
    UART_DataBits_t DataBits;
    UART_Parity_t Parity;
    CBFunc_t txCallback;
    CBFunc_t rxCallback;
} UART_Config_t;

STD_ReturnType UART_Init(const UART_Config_t* uartObj, SYSTICK_ClockSource_t clockSource);
STD_ReturnType UART_DeInit(const UART_Config_t* uartObj);
STD_ReturnType UART_SendChar(const UART_Config_t* uartObj, uint8_t data, uint32_t timeoutMS);
STD_ReturnType UART_SendBuffer(const UART_Config_t* uartObj, const uint8_t* data, uint8_t length, uint32_t timeoutMS);
STD_ReturnType UART_ReceiveChar(const UART_Config_t* uartObj, uint8_t* data, uint32_t timeoutMS);
STD_ReturnType UART_ReceiveBuffer(const UART_Config_t* uartObj, uint8_t* data, uint8_t length, uint32_t timeoutMS);

STD_ReturnType UART_SendCharIT(const UART_Config_t* uartObj, uint8_t data);
STD_ReturnType UART_SendBufferIT(const UART_Config_t* uartObj, const uint8_t* data, uint8_t length);
STD_ReturnType UART_ReceiveCharIT(const UART_Config_t* uartObj, uint8_t* data);
STD_ReturnType UART_ReceiveBufferIT(const UART_Config_t* uartObj, uint8_t* data, uint8_t length);

#endif // UART_H
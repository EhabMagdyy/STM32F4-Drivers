#include "interface/HAL/hserial.h"

STD_ReturnType HSerial_Init(HSerial_Config_t* hserialConfig, SYSTICK_ClockSource_t clockSource){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL || hserialConfig->uartConfig == NULL || hserialConfig->dmaConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = UART_Init(hserialConfig->uartConfig, clockSource);
        if(ret != STD_SUCCESS){
            return ret;
        }

        ret = DMA_Init(hserialConfig->dmaConfig);
        if(ret != STD_SUCCESS){
            return ret;
        }

        // DMA Enable
        hserialConfig->uartConfig->UartInstance->CR3 |=(1 << 7); // TX DMA Enable
        //hserialConfig->uartConfig->UartInstance->CR3 |=(1 << 6); // RX DMA Enable
    }

    return ret;
}


STD_ReturnType HSerial_DeInit(HSerial_Config_t* hserialConfig){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL || hserialConfig->uartConfig == NULL || hserialConfig->dmaConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = DMA_DeInit(hserialConfig->dmaConfig);
        if(ret != STD_SUCCESS){
            return ret;
        }

        ret = UART_DeInit(hserialConfig->uartConfig);
        if(ret != STD_SUCCESS){
            return ret;
        }
    }

    return ret;
}

STD_ReturnType HSerial_SendBuffer(HSerial_Config_t* hserialConfig){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL || hserialConfig->uartConfig == NULL || hserialConfig->dmaConfig == NULL || hserialConfig->buffer == NULL){
        ret = STD_ERROR;
    }
    else{        
        ret = DMA_Start(hserialConfig->dmaConfig, (uint32_t)(hserialConfig->buffer->src), (uint32_t)&(hserialConfig->uartConfig->UartInstance->DR), hserialConfig->buffer->length);
    }

    return ret;
}

// STD_ReturnType HSerial_ReceiveBuffer(HSerial_Config_t* hserialConfig){
//     STD_ReturnType ret = STD_SUCCESS;

//     if(hserialConfig == NULL || hserialConfig->uartConfig == NULL || hserialConfig->dmaConfig == NULL || hserialConfig->buffer == NULL){
//         ret = STD_ERROR;
//     }
//     else{        
//         // Start DMA Transfer
//         ret = DMA_Start(hserialConfig->dmaConfig, (uint32_t)&(hserialConfig->uartConfig->UartInstance->DR), (uint32_t)(hserialConfig->buffer->dest), hserialConfig->buffer->length);
//     }

//     return ret;
// }
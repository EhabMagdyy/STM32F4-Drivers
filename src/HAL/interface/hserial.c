#include "interface/HAL/hserial.h"

STD_ReturnType HSerial_Init(HSerial_Config_t* hserialConfig, SYSTICK_ClockSource_t clockSource){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = UART_Init(hserialConfig->uartConfig, clockSource);
        if(ret != STD_SUCCESS){
            return ret;
        }

        ret = DMA_Init(hserialConfig->txDma);
        if(ret != STD_SUCCESS){
            return ret;
        }

        ret = DMA_Init(hserialConfig->rxDma);
        if(ret != STD_SUCCESS){
            return ret;
        }

        // DMA Enable
        hserialConfig->uartConfig->UartInstance->CR3 |=(1 << 7); // TX DMA Enable
        hserialConfig->uartConfig->UartInstance->CR3 |=(1 << 6); // RX DMA Enable
    }

    return ret;
}


STD_ReturnType HSerial_DeInit(HSerial_Config_t* hserialConfig){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = DMA_DeInit(hserialConfig->txDma);
        if(ret != STD_SUCCESS){
            return ret;
        }

        ret = DMA_DeInit(hserialConfig->rxDma);
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

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{        
        ret = DMA_Start(hserialConfig->txDma, (uint32_t)(hserialConfig->txBuffer->src), 
                (uint32_t)&(hserialConfig->uartConfig->UartInstance->DR), hserialConfig->txBuffer->length);
    }

    return ret;
}

STD_ReturnType HSerial_ReceiveBuffer(HSerial_Config_t* hserialConfig){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{        
        // Start DMA Transfer
        ret = DMA_Start(hserialConfig->rxDma, (uint32_t)&(hserialConfig->uartConfig->UartInstance->DR), (uint32_t)(hserialConfig->rxBuffer->dest), hserialConfig->rxBuffer->length);
    }

    return ret;
}

STD_ReturnType HSerial_TxGetState(HSerial_Config_t* hserialConfig, DMA_State_t* state){
    STD_ReturnType ret = STD_SUCCESS;
    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = DMA_GetState(hserialConfig->txDma, state);
    }
    return ret;
}

STD_ReturnType HSerial_RxGetState(HSerial_Config_t* hserialConfig, DMA_State_t* state){
    STD_ReturnType ret = STD_SUCCESS;
    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = DMA_GetState(hserialConfig->rxDma, state);
    }
    return ret;
}
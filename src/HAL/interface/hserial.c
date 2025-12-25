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

        if(hserialConfig->uartConfig->dmaEnable == UART_DMA_ENABLE){
            ret = DMA_Init(hserialConfig->txDma);
            if(ret != STD_SUCCESS){
                return ret;
            }

            ret = DMA_Init(hserialConfig->rxDma);
            if(ret != STD_SUCCESS){
                return ret;
            }
        }
        else{
            // Nothing
        }
    }

    return ret;
}

STD_ReturnType HSerial_DeInit(HSerial_Config_t* hserialConfig){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        if(hserialConfig->uartConfig->dmaEnable == UART_DMA_ENABLE){
            ret = DMA_DeInit(hserialConfig->txDma);
            if(ret != STD_SUCCESS){
                return ret;
            }

            ret = DMA_DeInit(hserialConfig->rxDma);
            if(ret != STD_SUCCESS){
                return ret;
            }
        }
        else{
            // Nothing
        }

        ret = UART_DeInit(hserialConfig->uartConfig);
        if(ret != STD_SUCCESS){
            return ret;
        }
    }

    return ret;
}

STD_ReturnType HSerial_SendBuffer(HSerial_Config_t* hserialConfig, uint32_t timeoutMS){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = UART_SendBuffer(hserialConfig->uartConfig, (Buffer_t*)hserialConfig->txBuffer, timeoutMS);
    }

    return ret;
}

STD_ReturnType HSerial_SendBufferIT(HSerial_Config_t* hserialConfig){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = UART_SendBufferIT(hserialConfig->uartConfig, (Buffer_t*)hserialConfig->txBuffer);
    }

    return ret;
}

STD_ReturnType HSerial_SendBufferDMA(HSerial_Config_t* hserialConfig){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{        
        DMA_State_t state = 0;
        ret = DMA_GetState(hserialConfig->txDma, &state);
        if(state == DMA_STATE_READY){
            // Start DMA
            ret = DMA_Start(hserialConfig->txDma, (uint32_t)(hserialConfig->txBuffer->src), (uint32_t)&(hserialConfig->uartConfig->UartInstance->DR), hserialConfig->txBuffer->length);
        }
        else if(state == DMA_STATE_BUSY){
            ret = STD_BUSY;
        }
        else{
            ret = STD_ERROR;
        }
    }

    return ret;
}

STD_ReturnType HSerial_ReceiveBuffer(HSerial_Config_t* hserialConfig, uint32_t timeoutMS){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = UART_ReceiveBuffer(hserialConfig->uartConfig, (Buffer_t*)hserialConfig->rxBuffer, timeoutMS);
    }

    return ret;
}

STD_ReturnType HSerial_ReceiveBufferIT(HSerial_Config_t* hserialConfig){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = UART_ReceiveBufferIT(hserialConfig->uartConfig, (Buffer_t*)hserialConfig->rxBuffer);
    }

    return ret;
}

STD_ReturnType HSerial_ReceiveBufferDMA(HSerial_Config_t* hserialConfig){
    STD_ReturnType ret = STD_SUCCESS;

    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{        
        DMA_State_t state = 0;
        ret = DMA_GetState(hserialConfig->rxDma, &state);
        if(state == DMA_STATE_READY){
            // Start DMA
            ret = DMA_Start(hserialConfig->rxDma, (uint32_t)&(hserialConfig->uartConfig->UartInstance->DR), (uint32_t)(hserialConfig->rxBuffer->dest), hserialConfig->rxBuffer->length);
        }
        else if(state == DMA_STATE_BUSY){
            ret = STD_BUSY;
        }
        else{
            ret = STD_ERROR;
        }
    }

    return ret;
}

STD_ReturnType HSerial_TxGetStateDMA(HSerial_Config_t* hserialConfig, DMA_State_t* state){
    STD_ReturnType ret = STD_SUCCESS;
    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = DMA_GetState(hserialConfig->txDma, state);
    }
    return ret;
}

STD_ReturnType HSerial_RxGetStateDMA(HSerial_Config_t* hserialConfig, DMA_State_t* state){
    STD_ReturnType ret = STD_SUCCESS;
    if(hserialConfig == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = DMA_GetState(hserialConfig->rxDma, state);
    }
    return ret;
}
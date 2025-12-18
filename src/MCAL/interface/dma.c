#include "interface/MCAL/dma.h"
#include "interface/MCAL/rcc.h"
#include "private/MCAL/dma_priv.h"
#include "interface/Core/nvic.h"

#define DMA_STREAM  DMA[dmaInstance->dmaNum]->STREAM[dmaInstance->stream]
#define DMA_CR_EN_BIT  0

DMA_Registers_t* DMA[2] = {DMA1, DMA2};

STD_ReturnType DMA_Init(DMA_Instance_t* dmaInstance){
    STD_ReturnType ret = STD_SUCCESS;
    if(dmaInstance == NULL || dmaInstance->stream > 7 || dmaInstance->channel > 7){
        ret = STD_ERROR;
    }
    else{
        // 1. Enable Peripheral Clock
        if(dmaInstance->dmaNum == DMA_1){
            ret = RCC_ControlPeripheral(RCC_DMA1, RCC_PERIPHERAL_ENABLE);
        }
        else if(dmaInstance->dmaNum == DMA_2){
            ret = RCC_ControlPeripheral(RCC_DMA2, RCC_PERIPHERAL_ENABLE);
        }
        else{
            ret = STD_ERROR;
        }

        // 2. Configure Stream & Channel
        DMA_STREAM.CR |= (dmaInstance->channel << 25);
        
        // 3. Configure Direction
        DMA_STREAM.CR |= (dmaInstance->direction << 6);
        
        // 4. Memory increment
        DMA_STREAM.CR |= (dmaInstance->memInc << 10);

        // 5. Peripheral increment
        DMA_STREAM.CR |= (dmaInstance->periphInc << 9);

        // 6. Memory Burst
        DMA_STREAM.CR |= (dmaInstance->memBurst << 23);

        // 7. Peripheral Burst
        DMA_STREAM.CR |= (dmaInstance->periphBurst << 21);

        // 8. Priority
        DMA_STREAM.CR |= (dmaInstance->priority << 16);

        // 9. NVIC Enable
        if(dmaInstance->dmaNum == DMA_1){
            ret = NVIC_EnableIRQ(DMA1_Stream0_IRQn);
        }
        else if(dmaInstance->dmaNum == DMA_2){
            ret = NVIC_EnableIRQ(DMA2_Stream0_IRQn);
        }
        else{
            ret = STD_ERROR;
        }
    }
    return ret;
}

STD_ReturnType DMA_DeInit(DMA_Instance_t* dmaInstance){
    STD_ReturnType ret = STD_SUCCESS;

    if(dmaInstance == NULL || dmaInstance->stream > 7 || dmaInstance->channel > 7){
        ret = STD_ERROR;
    }
    else{
        DMA_STREAM.CR &= ~(1 << DMA_CR_EN_BIT);
 
        DMA_STREAM.CR = 0U;
        DMA_STREAM.NDTR = 0U;
        DMA_STREAM.PAR = 0U;
        DMA_STREAM.M0AR = 0U;
        DMA_STREAM.M1AR = 0U;
        DMA_STREAM.FCR = 0U;

        // Disable Peripheral Clock
        if(dmaInstance->dmaNum == DMA_1){
            ret = RCC_ControlPeripheral(RCC_DMA1, RCC_PERIPHERAL_DISABLE);
        }
        else if(dmaInstance->dmaNum == DMA_2){
            ret = RCC_ControlPeripheral(RCC_DMA2, RCC_PERIPHERAL_DISABLE);
        }
        else{
            ret = STD_ERROR;
        }

        // NVIC Disable
        if(dmaInstance->dmaNum == DMA_1){
            ret = NVIC_DisableIRQ(DMA1_Stream0_IRQn);
        }
        else if(dmaInstance->dmaNum == DMA_2){
            ret = NVIC_DisableIRQ(DMA2_Stream0_IRQn);
        }
        else{
            ret = STD_ERROR;
        }
    }

    return ret;
}

STD_ReturnType DMA_Start(DMA_Instance_t* dmaInstance, uint8_t* src, uint8_t* dest, uint8_t length){
    STD_ReturnType ret = STD_SUCCESS;

    if(dmaInstance == NULL || dmaInstance->stream > 7 || dmaInstance->channel > 7){
        ret = STD_ERROR;
    }
    else{
        DMA_STREAM.CR &= ~(1 << DMA_CR_EN_BIT);

        DMA_STREAM.NDTR = length;
        if(dmaInstance->direction == DMA_MEMORY_TO_PERIPHERAL){
            DMA_STREAM.PAR = (uint32_t)dest;
            DMA_STREAM.M0AR = (uint32_t)src;
        }
        else{
            DMA_STREAM.PAR = (uint32_t)src;
            DMA_STREAM.M0AR = (uint32_t)dest;
        }

        DMA_STREAM.CR |= (1 << DMA_CR_EN_BIT);
    }

    return ret;
}
#include "interface/HAL/hspi.h"
#include "private/MCAL/spi_priv.h"

STD_ReturnType HSPI_Init(const HSPI_DMA_t* config, SYSTICK_ClockSource_t clockSource){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = SPI_Init(config->spiConfig, clockSource);
        if(ret != STD_SUCCESS){
            return ret;
        }

        if(config->spiConfig->dmaEnable == SPI_DMA_ENABLE){
            // Configure DMA Channels
            ret = DMA_Init(config->txChannel);
            if(ret != STD_SUCCESS){
                return ret;
            }
            ret = DMA_Init(config->rxChannel);
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
STD_ReturnType HSPI_DeInit(const HSPI_DMA_t* config){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL){
        ret = STD_ERROR;
    }
    else{
        if(config->spiConfig->dmaEnable == SPI_DMA_ENABLE){
            // DeInitialize DMA Channels
            ret = DMA_DeInit(config->txChannel);
            if(ret != STD_SUCCESS){
                return ret;
            }
            ret = DMA_DeInit(config->rxChannel);
            if(ret != STD_SUCCESS){
                return ret;
            }
        }
        else{
            // Nothing
        }

        ret = SPI_DeInit(config->spiConfig);
    }

    return ret;
}

STD_ReturnType HSPI_StartTranceive(const HSPI_DMA_t* config, SPI_Buffer_t* buffer, uint32_t timeout){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL) {
        ret = STD_ERROR;
    }
    else{
        ret = SPI_Tranceive(config->spiConfig, buffer, timeout);
    }

    return ret;
}

STD_ReturnType HSPI_StartTranceiveIT(const HSPI_DMA_t* config, SPI_Buffer_t* buffer){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL) {
        ret = STD_ERROR;
    }
    else{
        ret = SPI_TranceiveIT(config->spiConfig, buffer);
    }

    return ret;
}

STD_ReturnType HSPI_StartTranceiveDMA(const HSPI_DMA_t* config, SPI_Buffer_t* buffer){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL || buffer == NULL || buffer->txData == NULL || buffer->rxData == NULL || buffer->length == 0) {
        ret = STD_ERROR;
    }
    else{
        // Start RX DMA
        ret = DMA_Start(config->rxChannel, (uint32_t)&(SPI1->DR), (uint32_t)(buffer->rxData), buffer->length);
        if(ret != STD_SUCCESS){
            return ret;
        }

        // Start TX DMA
        ret = DMA_Start(config->txChannel, (uint32_t)(buffer->txData), (uint32_t)&(SPI1->DR), buffer->length);
    }

    return ret;
}
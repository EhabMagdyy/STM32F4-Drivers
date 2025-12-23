#include "interface/HAL/hspi.h"

volatile SPI_Buffer_t* spiBuf[4];

SPI_Callback_t SPI1_Callback[4] = {NULL};
volatile static uint8_t whoIsNext[4] = {0};

void SPI_ITHandler(uint8_t spiNum){
    uint8_t flagStatus = 0;
    SPI_GetTXEFlag(spiNum, &flagStatus);
    if(flagStatus && whoIsNext[spiNum] == 0){ // TXE Interrupt
        if(spiBuf[spiNum]->txIndex < spiBuf[spiNum]->length){
            // Send next data
            flagStatus = 0;
            SPI_GetDataSize(spiNum, &flagStatus);
            if(flagStatus == SPI_DATA_FRAME_16BIT){
                SPI_SetDR16Bits(spiNum, (uint8_t*)&(spiBuf[spiNum]->txData[spiBuf[spiNum]->txIndex]));
                
                spiBuf[spiNum]->txIndex += 2;
            }
            else{
                SPI_SetDR8Bits(spiNum, (uint8_t*)&(spiBuf[spiNum]->txData[spiBuf[spiNum]->txIndex]));
                spiBuf[spiNum]->txIndex += 1;
            }
            whoIsNext[spiNum] = 1;
        }
        else{
            // All data transmitted -> wait for receiveing to complete
        }
    }
    flagStatus = 0;
    SPI_GetRXNEFlag(spiNum, &flagStatus);
    if(flagStatus && whoIsNext[spiNum] == 1){ // RXNE Interrupt
        if(spiBuf[spiNum]->rxIndex < spiBuf[spiNum]->length){
            // Receive next data
            flagStatus = 0;
            SPI_GetDataSize(spiNum, &flagStatus);
            if(flagStatus == SPI_DATA_FRAME_16BIT){
                SPI_GetDR16Bits(spiNum, (uint8_t*)&(spiBuf[spiNum]->rxData[spiBuf[spiNum]->rxIndex]));
                spiBuf[spiNum]->rxIndex += 2;
            }
            else{
                SPI_GetDR8Bits(spiNum, (uint8_t*)&(spiBuf[spiNum]->rxData[spiBuf[spiNum]->rxIndex]));
                spiBuf[spiNum]->rxIndex += 1;
            }
            whoIsNext[spiNum] = 0;
            // if all received -> disable rxne & set state to ready & call callback
            if(spiBuf[spiNum]->rxIndex >= spiBuf[spiNum]->length){
                // Disable TXE & RXNE Interrupt & NVIC IRQ
                SPI_NVIC_TXE_RXNE_Disable(spiNum);
                SPI_SetState(spiNum, SPI_STATE_READY);
                if(NULL != SPI1_Callback[spiNum]){
                    SPI1_Callback[spiNum]();
                }
            }
        }
        else{

        }
    }
}

void SPI1_ITHandler(void){
    SPI_ITHandler(SPI_1);
}

void SPI2_ITHandler(void){
    SPI_ITHandler(SPI_2);
}

void SPI3_ITHandler(void){
    SPI_ITHandler(SPI_3);
}

void SPI4_ITHandler(void){
    SPI_ITHandler(SPI_4);
}

STD_ReturnType HSPI_Init(const HSPI_DMA_t* config){
    STD_ReturnType ret = STD_SUCCESS;

    if(config == NULL){
        ret = STD_ERROR;
    }
    else{
        ret = SPI_Init(config->spiConfig);
        // Set Callback Function
        SPI1_Callback[config->spiConfig->spiNum] = config->spiConfig->trancieveCallback;
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
        if(ret == STD_SUCCESS){
            spiBuf[config->spiConfig->spiNum] = buffer;
        }
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
        ret = DMA_Start(config->rxChannel, SPI_DRAddress(config->spiConfig->spiNum), (uint32_t)(buffer->rxData), buffer->length);
        if(ret != STD_SUCCESS){
            return ret;
        }

        // Start TX DMA
        ret = DMA_Start(config->txChannel, (uint32_t)(buffer->txData), SPI_DRAddress(config->spiConfig->spiNum), buffer->length);
    }

    return ret;
}
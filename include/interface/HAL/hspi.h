#ifndef HAL_HSPI_H
#define HAL_HSPI_H

#include "interface/MCAL/spi.h"
#include "interface/MCAL/dma.h"

typedef struct{
    SPI_Config_t* spiConfig;
    DMA_Instance_t* txChannel;
    DMA_Instance_t* rxChannel;
} HSPI_DMA_t;

STD_ReturnType HSPI_Init(const HSPI_DMA_t* config, SYSTICK_ClockSource_t clockSource);
STD_ReturnType HSPI_DeInit(const HSPI_DMA_t* config);
STD_ReturnType HSPI_StartTranceive(const HSPI_DMA_t* config, SPI_Buffer_t* buffer);

#endif // HAL_HSPI_H
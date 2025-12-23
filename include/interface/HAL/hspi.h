#ifndef HAL_HSPI_H
#define HAL_HSPI_H

#include "interface/MCAL/spi.h"
#include "interface/MCAL/dma.h"

typedef struct{
    SPI_Config_t* spiConfig;
    DMA_Instance_t* txChannel;
    DMA_Instance_t* rxChannel;
} HSPI_DMA_t;

/**
 * @brief  Initialize SPI peripheral with DMA support.
 *
 * @param[in]  config       Pointer to HSPI DMA configuration structure.
 *
 * @retval STD_SUCCESS  Initialization completed successfully.
 * @retval STD_ERROR    Invalid configuration pointer or initialization failure.
 */
STD_ReturnType HSPI_Init(const HSPI_DMA_t* config);

/**
 * @brief  De-initialize SPI peripheral and associated DMA channels.
 *
 * @param[in]  config  Pointer to HSPI DMA configuration structure.
 *
 * @retval STD_SUCCESS  De-initialization completed successfully.
 * @retval STD_ERROR    Invalid configuration pointer or de-initialization failure.
 */
STD_ReturnType HSPI_DeInit(const HSPI_DMA_t* config);

/**
 * @brief  Start blocking SPI transmit/receive operation.
 *
 * @param[in]  config   Pointer to HSPI DMA configuration structure.
 * @param[in]  buffer   Pointer to SPI buffer structure containing TX and RX data.
 * @param[in]  timeout  Timeout value in milliseconds.
 *
 * @retval STD_SUCCESS   Transfer completed successfully.
 * @retval STD_TIMEOUT   Operation timed out.
 * @retval STD_ERROR     Invalid parameters.
 */
STD_ReturnType HSPI_StartTranceive(const HSPI_DMA_t* config, SPI_Buffer_t* buffer, uint32_t timeout);

/**
 * @brief  Start non-blocking SPI transmit/receive operation using interrupts.
 *
 * @param[in]  config  Pointer to HSPI DMA configuration structure.
 * @param[in]  buffer  Pointer to SPI buffer structure containing TX and RX data.
 *
 * @retval STD_SUCCESS  Transfer started successfully.
 * @retval STD_BUSY     SPI peripheral is busy.
 * @retval STD_ERROR    Invalid parameters.
 */
STD_ReturnType HSPI_StartTranceiveIT(const HSPI_DMA_t* config, SPI_Buffer_t* buffer);

/**
 * @brief  Start non-blocking SPI transmit/receive operation using DMA.
 *
 * @param[in]  config  Pointer to HSPI DMA configuration structure.
 * @param[in]  buffer  Pointer to SPI buffer structure containing TX and RX data.
 *
 * @retval STD_SUCCESS  Transfer started successfully.
 * @retval STD_BUSY     SPI peripheral is busy.
 * @retval STD_ERROR    Invalid parameters.
 */
STD_ReturnType HSPI_StartTranceiveDMA(const HSPI_DMA_t* config, SPI_Buffer_t* buffer);


#endif // HAL_HSPI_H
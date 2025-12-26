#ifndef SPI_PRIV_H
#define SPI_PRIV_H

#include "../../../lib/STD_Types.h"

typedef struct{
    volatile uint32_t CR1;      // 0x00
    volatile uint32_t CR2;      // 0x04
    volatile uint32_t SR;       // 0x08
    volatile uint32_t DR;       // 0x0C
    volatile uint32_t CRCPR;    // 0x10
    volatile uint32_t RXCRCR;   // 0x14
    volatile uint32_t TXCRCR;   // 0x18
} SPI_Reg_t;

#define SPI1   ((SPI_Reg_t*)0x40013000UL)
#define SPI2   ((SPI_Reg_t*)0x40003800UL)
#define SPI3   ((SPI_Reg_t*)0x40003C00UL)
#define SPI4   ((SPI_Reg_t*)0x40013400UL)

typedef SPI_Reg_t* SPI_Instance_t;

#endif // SPI_PRIV_H
#ifndef I2C_PRIV_H
#define I2C_PRIV_H

#include "../../../lib/STD_Types.h"

#define I2C1_BASE_ADDRESS 0x40005400
#define I2C2_BASE_ADDRESS 0x40005800
#define I2C3_BASE_ADDRESS 0x40005C00

typedef struct I2C_Registers {
    volatile uint32_t CR1;      // Control register 1 (0x00)
    volatile uint32_t CR2;      // Control register 2 (0x04)
    volatile uint32_t OAR1;     // Own address register 1 (0x08)
    volatile uint32_t OAR2;     // Own address register 2 (0x0C)
    volatile uint32_t DR;       // Data register (0x10)
    volatile uint32_t SR1;      // Status register 1 (0x14)
    volatile uint32_t SR2;      // Status register 2 (0x18)
    volatile uint32_t CCR;      // Clock control register (0x1C)
    volatile uint32_t TRISE;    // TRISE register (0x20)
    volatile uint32_t FLTR;     // Filter register (0x24)
} I2C_Registers_t;

#define I2C1 ((I2C_Registers_t*)I2C1_BASE_ADDRESS)
#define I2C2 ((I2C_Registers_t*)I2C2_BASE_ADDRESS)
#define I2C3 ((I2C_Registers_t*)I2C3_BASE_ADDRESS)

#endif // I2C_PRIV_H
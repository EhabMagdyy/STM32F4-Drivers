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

/* CR1 bits */
#define I2C_CR1_PE        (1 << 0)
#define I2C_CR1_SMBUS     (1 << 1)
#define I2C_CR1_START     (1 << 8)
#define I2C_CR1_STOP      (1 << 9)
#define I2C_CR1_ACK       (1 << 10)
#define I2C_CR1_POS       (1 << 11)
#define I2C_CR1_PEC       (1 << 12)
#define I2C_CR1_ALERT     (1 << 13)
#define I2C_CR1_SWRST     (1 << 15)

/* CR2 bits */
#define I2C_CR2_FREQ_MASK 0x3F
#define I2C_CR2_ITERREN   (1 << 8)
#define I2C_CR2_ITEVTEN   (1 << 9)
#define I2C_CR2_ITBUFEN   (1 << 10)
#define I2C_CR2_DMAEN     (1 << 11)
#define I2C_CR2_LAST      (1 << 12)

/* OAR1 bits */
#define I2C_OAR1_ADDMODE  (1 << 15)

/* OAR2 bits */
#define I2C_OAR2_ENDUAL   (1 << 0)

/* SR1 bits */
#define I2C_SR1_SB        (1 << 0)
#define I2C_SR1_ADDR      (1 << 1)
#define I2C_SR1_BTF       (1 << 2)
#define I2C_SR1_ADD10     (1 << 3)
#define I2C_SR1_STOPF     (1 << 4)
#define I2C_SR1_RxNE      (1 << 6)
#define I2C_SR1_TxE       (1 << 7)
#define I2C_SR1_BERR      (1 << 8)
#define I2C_SR1_ARLO      (1 << 9)
#define I2C_SR1_AF        (1 << 10)
#define I2C_SR1_OVR       (1 << 11)
#define I2C_SR1_PECERR    (1 << 12)
#define I2C_SR1_TIMEOUT   (1 << 14)
#define I2C_SR1_SMBALERT  (1 << 15)

/* SR1 bits */
#define I2C_SR2_MSL       (1 << 0)
#define I2C_SR2_BUSY      (1 << 1)
#define I2C_SR2_TRA       (1 << 2)
#define I2C_SR2_GENCALL   (1 << 4)
#define I2C_SR2_SMBDEFAULT (1 << 5)
#define I2C_SR2_SMBHOST   (1 << 6)
#define I2C_SR2_DUALF     (1 << 7)

/* CCR bits */
#define I2C_CCR_FS        (1 << 15)
#define I2C_CCR_DUTY      (1 << 14)

#endif // I2C_PRIV_H
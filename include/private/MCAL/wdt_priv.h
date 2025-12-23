#ifndef WDT_PRIV_H
#define WDT_PRIV_H

#include "../../../lib/STD_Types.h"

#define WDT_BASE_ADDR      0x40003000UL

#define WDT_KEY_RELOAD       0xAAAAU
#define WDT_KEY_START        0xCCCCU
#define WDT_KEY_ACCESS       0x5555U

typedef struct{
    volatile uint32_t KR;       // Key Register
    volatile uint32_t PR;       // Prescaler Register
    volatile uint32_t RLR;      // Reload Register
    volatile uint32_t SR;       // Status Register
} WDT_Reg_t;

#define WDT     ((WDT_Reg_t *)WDT_BASE_ADDR)

#endif // WDT_PRIV_H
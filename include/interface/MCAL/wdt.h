#ifndef WDT_H
#define WDT_H

#include "../../../lib/STD_Types.h"

typedef enum{
    WDT_PRESCALER_4 = 0,
    WDT_PRESCALER_8,
    WDT_PRESCALER_16,
    WDT_PRESCALER_32,   // 1 Tick = 1ms
    WDT_PRESCALER_64,
    WDT_PRESCALER_128,
    WDT_PRESCALER_256
} WDT_Prescaler_t;

STD_ReturnType WDT_Init(WDT_Prescaler_t prescaler, uint16_t reloadValue);
STD_ReturnType WDT_Refresh(void);
STD_ReturnType WDT_Start(void);

#endif // WDT_H
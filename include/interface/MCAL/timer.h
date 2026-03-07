#ifndef TIMER_H
#define TIMER_H

#include "../../../lib/STD_Types.h"

typedef enum {
    TIMER_2,
    TIMER_3,
    TIMER_4,
    TIMER_5
} Timer_Instance_t;

typedef enum {
    TIMER_MODE_UP,
    TIMER_MODE_DOWN,
    TIMER_MODE_CENTER_ALIGNED_1,
    TIMER_MODE_CENTER_ALIGNED_2,
    TIMER_MODE_CENTER_ALIGNED_3
} Timer_Mode_t;

typedef enum {
    TIMER_CLOCK_DIV_1,
    TIMER_CLOCK_DIV_2,
    TIMER_CLOCK_DIV_4
} Timer_ClockDivision_t;

typedef enum {
    TIMER_CHANNEL_1,
    TIMER_CHANNEL_2,
    TIMER_CHANNEL_3,
    TIMER_CHANNEL_4
} Timer_Channel_t;

typedef struct {
    Timer_Instance_t instance;
    Timer_Mode_t mode;
    Timer_ClockDivision_t clockDivision;
    uint16_t prescaler;
    uint32_t autoReloadValue;
} Timer_t;

STD_ReturnType Timer_Init(const Timer_t *timerConfig);
STD_ReturnType Timer_Start(const Timer_t *timerConfig);
STD_ReturnType Timer_Stop(const Timer_t *timerConfig);
STD_ReturnType Timer_GetCounter(const Timer_t *timerConfig, uint32_t* counterValue);
STD_ReturnType Timer_SetAutoReload(const Timer_t *timerConfig);
STD_ReturnType Timer_SetPrescaler(const Timer_t *timerConfig);
STD_ReturnType Timer_SetClockDivision(const Timer_t *timerConfig);
STD_ReturnType Timer_SetMode(const Timer_t *timerConfig);

#endif // TIMER_H
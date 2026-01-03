#ifndef OS_SCHEDULER_H
#define OS_SCHEDULER_H

#include "../lib/STD_Types.h"
#include "interface/Core/systick.h"

#define MAX_RUNNABLES 10

typedef struct {
    void (*callback)(void* arg);
    uint32_t Periodicity;
    uint8_t FirstDelay;
    void* arg;
} Runnable__t;

STD_ReturnType Scheduler_Init(SYSTICK_ClockSource_t clockSource ,uint32_t tickTimeMS);
STD_ReturnType Scheduler_RegisterRunnable(Runnable__t* runnable);
STD_ReturnType Scheduler_Start(void);
STD_ReturnType Scheduler_Stop(void);

#endif // OS_SCHEDULER_H
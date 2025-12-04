#include "OS/scheduler.h"

static volatile Runnable__t* runnables[MAX_RUNNABLES];
static volatile uint32_t runnableCount = 0;

volatile uint8_t schedStartFlag = 0;

STD_ReturnType Scheduler_Init(SYSTICK_ClockSource_t clockSource ,uint32_t tickTimeMS){
    STD_ReturnType ret = STD_SUCCESS;

    // Initialize SYSTICK
    ret = SYSTICK_Init(clockSource);
    // Set Tick Time & Periodic interrupt
    ret = SYSTICK_PeriodicInterval(tickTimeMS, NULL);

    return ret;
}

STD_ReturnType Scheduler_RegisterRunnable(Runnable__t* runnable){
    STD_ReturnType ret = STD_SUCCESS;

    if(runnableCount >= MAX_RUNNABLES){
        ret = STD_ERROR;
    }
    else{
        runnables[runnableCount++] = runnable;
    }

    return ret;
}

STD_ReturnType Scheduler_Start(void){
    STD_ReturnType ret = STD_SUCCESS;
    static volatile uint32_t counter = 0;

    ret = SYSTICK_Enable();

    while(1){
        if(schedStartFlag){
            for(volatile uint8_t i = 0; i < runnableCount; i++){
                if(runnables[i]->FirstDelay > 0){
                    runnables[i]->FirstDelay--;
                    if(runnables[i]->FirstDelay == 0){
                        runnables[i]->callback((void*)&(runnables[i]->arg));
                    }
                }
                else if(counter % (runnables[i]->Periodicity) == 0){
                    runnables[i]->callback((void*)&(runnables[i]->arg));
                    runnables[i]->FirstDelay = 0;
                }
            }
            schedStartFlag = 0;
            counter++;
        }
        else{
            // Do nothing, wait for the scheduler to start
        }
    }

    return ret;
}

STD_ReturnType Scheduler_Stop(void){
    STD_ReturnType ret = STD_SUCCESS;

    ret = SYSTICK_Stop();

    return ret;
}
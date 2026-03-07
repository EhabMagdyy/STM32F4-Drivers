#include "private/MCAL/timer_priv.h"
#include "interface/MCAL/timer.h"
#include "interface/MCAL/rcc.h"

TIM_TypeDef* TIM[4] = {TIM2, TIM3, TIM4, TIM5};

STD_ReturnType Timer_Init(const Timer_t *timerConfig){
    if(timerConfig == NULL){
        return STD_ERROR;
    }
    // Enable the clock for the specified timer
    switch(timerConfig->instance){
        case TIMER_2:
            RCC_ControlPeripheral(RCC_TIM2, RCC_PERIPHERAL_ENABLE);
            break;
        case TIMER_3:
            RCC_ControlPeripheral(RCC_TIM3, RCC_PERIPHERAL_ENABLE);
            break;
        case TIMER_4:
            RCC_ControlPeripheral(RCC_TIM4, RCC_PERIPHERAL_ENABLE);
            break;
        case TIMER_5:
            RCC_ControlPeripheral(RCC_TIM5, RCC_PERIPHERAL_ENABLE);
            break;
        default:
            return STD_ERROR;
    }

    // Configure the timer based on the provided configuration
    Timer_SetMode(timerConfig);
    Timer_SetClockDivision(timerConfig);
    Timer_SetPrescaler(timerConfig);
    Timer_SetAutoReload(timerConfig);
    // Generate an update event to load the prescaler value immediately
    TIM[timerConfig->instance]->EGR |= (1U << 0);   // Trigger update
    TIM[timerConfig->instance]->SR  &= ~(1U << 0);  // clear UIF

    return STD_SUCCESS;
}

STD_ReturnType Timer_Start(const Timer_t *timerConfig){
    if(timerConfig == NULL){
        return STD_ERROR;
    }
    else{
        TIM[timerConfig->instance]->CR1 |= (1U << 0); // Set the CEN bit to start the timer
    }

    return STD_SUCCESS;
}

STD_ReturnType Timer_Stop(const Timer_t *timerConfig){
    if(timerConfig == NULL){
        return STD_ERROR;
    }
    else{
        TIM[timerConfig->instance]->CR1 &= ~(1U << 0); // Clear the CEN bit to stop the timer
    }

    return STD_SUCCESS;
}

STD_ReturnType Timer_GetCounter(const Timer_t *timerConfig, uint32_t* counterValue){
    if(timerConfig == NULL){
        return STD_ERROR;
    }
    else{
        *counterValue = TIM[timerConfig->instance]->CNT;
    }

    return STD_SUCCESS;
}

STD_ReturnType Timer_SetAutoReload(const Timer_t *timerConfig){
    if(timerConfig == NULL){
        return STD_ERROR;
    }
    else{
        TIM[timerConfig->instance]->ARR = timerConfig->autoReloadValue;
    }

    return STD_SUCCESS;
}

STD_ReturnType Timer_SetPrescaler(const Timer_t *timerConfig){
    if(timerConfig == NULL){
        return STD_ERROR;
    }
    else{
        TIM[timerConfig->instance]->PSC = timerConfig->prescaler;
    }

    return STD_SUCCESS;
}

STD_ReturnType Timer_SetClockDivision(const Timer_t *timerConfig){
    if(timerConfig == NULL){
        return STD_ERROR;
    }
    else{
        TIM[timerConfig->instance]->CR1 &= ~(0x3U << 8); // Clear the CKD bits
        TIM[timerConfig->instance]->CR1 |= (timerConfig->clockDivision << 8); // Set the CKD bits based on the clock division
    }

    return STD_SUCCESS;
}

STD_ReturnType Timer_SetMode(const Timer_t *timerConfig){
    if(timerConfig == NULL){
        return STD_ERROR;
    }
    else{
        if(timerConfig->mode == TIMER_MODE_UP || timerConfig->mode == TIMER_MODE_DOWN){
            TIM[timerConfig->instance]->CR1 &= ~(0x3U << 5); // Clear the CMS bits
            TIM[timerConfig->instance]->CR1 &= ~(1U << 4);
            TIM[timerConfig->instance]->CR1 |= (timerConfig->mode << 4); // 0 => up, 1 => down
        }
        else{
            TIM[timerConfig->instance]->CR1 &= ~(0x3U << 5);
            TIM[timerConfig->instance]->CR1 |= ((timerConfig->mode - 1) << 5);  // Set the CMS bits based on the center-aligned mode
        }
    }

    return STD_SUCCESS;
}

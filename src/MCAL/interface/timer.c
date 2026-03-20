#include "private/MCAL/timer_priv.h"
#include "interface/MCAL/timer.h"
#include "interface/MCAL/rcc.h"
#include "interface/Core/nvic.h"

TIM_TypeDef* TIM[4] = {TIM2, TIM3, TIM4, TIM5};
Timer_Callback_t Timer_Callbacks[4] = {NULL, NULL, NULL, NULL}; // Array to hold callbacks for each timer instance

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
    Timer_SetPrescaler(timerConfig);
    Timer_SetAutoReload(timerConfig);
    // Generate an update event to load the prescaler value immediately
    TIM[timerConfig->instance]->EGR |= (1U << 0);   // (UG Bit) Update Generation
    TIM[timerConfig->instance]->SR  &= ~(1U << 0);  // clear UIF bit - Update Interrupt Flag

    // Set Callback
    if(timerConfig->callback != NULL){
        Timer_Callbacks[timerConfig->instance] = timerConfig->callback;
    }

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

STD_ReturnType Timer_Start_IT(const Timer_t *timerConfig){
    if(timerConfig == NULL || timerConfig->callback == NULL){
        return STD_ERROR;
    }
    else{
        // Enable Timer Update Interrupt
        TIM[timerConfig->instance]->DIER |= (1U << 0);
        // Enable NVIC for Timer

        switch(timerConfig->instance){
            case TIMER_2: NVIC_EnableIRQ(TIM2_IRQn); break;
            case TIMER_3: NVIC_EnableIRQ(TIM3_IRQn); break;
            case TIMER_4: NVIC_EnableIRQ(TIM4_IRQn); break;
            case TIMER_5: NVIC_EnableIRQ(TIM5_IRQn); break;
            default: return STD_ERROR;
        }
        // Start the timer
        TIM[timerConfig->instance]->CR1 |= (1U << 0);
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

STD_ReturnType Timer_Stop_IT(const Timer_t *timerConfig){
    if(timerConfig == NULL){
        return STD_ERROR;
    }
    else{
        // Disable Timer Update Interrupt
        TIM[timerConfig->instance]->DIER &= ~(1U << 0);
        // Disable NVIC for Timer
        switch(timerConfig->instance){
            case TIMER_2: NVIC_DisableIRQ(TIM2_IRQn); break;
            case TIMER_3: NVIC_DisableIRQ(TIM3_IRQn); break;
            case TIMER_4: NVIC_DisableIRQ(TIM4_IRQn); break;
            case TIMER_5: NVIC_DisableIRQ(TIM5_IRQn); break;
            default: return STD_ERROR;
        }
        // Stop the timer
        TIM[timerConfig->instance]->CR1 &= ~(1U << 0);
    }

    return STD_SUCCESS;
}

STD_ReturnType Timer_GetCounter(const Timer_t *timerConfig, uint32_t* counterValue){
    if(timerConfig == NULL || counterValue == NULL){
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
        if((timerConfig->instance == TIMER_3 || timerConfig->instance == TIMER_4) && (timerConfig->autoReloadValue > 0xFFFF)){
            TIM[timerConfig->instance]->ARR = 0xFFFF; // Set to max for 16-bit timers
        }
        else{
            TIM[timerConfig->instance]->ARR = timerConfig->autoReloadValue;
        }
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

void TIM_IRQ_Handler(Timer_Instance_t timerInstance){
    if(timerInstance < TIMER_2 || timerInstance > TIMER_5){
        return;
    }
    TIM_TypeDef* timer = TIM[timerInstance];
    // Check for update interrupt
    if(timer->SR & (1U << 0)){ 
        timer->SR &= ~(1U << 0); // Clear the update interrupt flag
        if(Timer_Callbacks[timerInstance] != NULL){
            Timer_Callbacks[timerInstance](); // Call callback function
        }
    }
}

void TIM2_IRQHandler(void){
    TIM_IRQ_Handler(TIMER_2);
}


void TIM3_IRQHandler(void){
    TIM_IRQ_Handler(TIMER_3);
}


void TIM4_IRQHandler(void){
    TIM_IRQ_Handler(TIMER_4);
}


void TIM5_IRQHandler(void){
    TIM_IRQ_Handler(TIMER_5);
}

#ifndef TIMER_H
#define TIMER_H

/** Note:
    Timers by default doesn't stop when the core is halted by the debugger,
    if you need to stop the timer when debugging,
    see the DBGMCU Register and set the corresponding bits for the timers you want to stop during debug halt. 
**/

#include "../../../lib/STD_Types.h"
#include "interface/MCAL/gpio.h"

typedef enum {
    TIMER_2,    // TIM2 is a 32-bit timer
    TIMER_3,    // TIM3 is a 16-bit timer
    TIMER_4,    // TIM4 is a 16-bit timer
    TIMER_5     // TIM5 is a 32-bit timer
} Timer_Instance_t;

typedef enum {
    TIMER_MODE_UP,
    TIMER_MODE_DOWN,
    TIMER_MODE_CENTER_ALIGNED_1,
    TIMER_MODE_CENTER_ALIGNED_2,
    TIMER_MODE_CENTER_ALIGNED_3
} Timer_Mode_t;

// All Four Channels share:
// - One counter (CNT)
// - Prescaler (PSC)
// - Auto-reload value (ARR)
// They differ in:
// - its own mode (Output Compare(PWM), Input Capture(input pwm)) which is configured through CCMR1 and CCMR2 registers
// - Capture/Compare values (CCR1, CCR2, CCR3, CCR4)
// - Interrupts for each channel (CC1IE, CC2IE, CC3IE, CC4IE)
typedef enum {
    TIMER_CHANNEL_1,
    TIMER_CHANNEL_2,
    TIMER_CHANNEL_3,
    TIMER_CHANNEL_4
} Timer_Channel_t;

typedef void (*Timer_Callback_t)(void);

typedef struct {
    Timer_Instance_t instance;      // Timer instance (TIMER_2, TIMER_3, TIMER_4, or TIMER_5)
    Timer_Mode_t mode;              // Timer counting mode (up, down, or center-aligned)
    uint16_t prescaler;             // Prescaler value (1 to 65535)
    uint32_t autoReloadValue;       // Auto-reload value (1 to 0xFFFFFFFF)
    Timer_Channel_t channel;        // Timer channel for Capture/Compare(PWM) operations not used in basic timer mode
    Timer_Callback_t callback;      // Callback function for timer interrupts
    GPIO_Port_t port;               // GPIO port for Capture/Compare (not used in basic timer mode)
    GPIO_Pin_t pin;                 // GPIO pin for Capture/Compare (not used in basic timer mode)
} Timer_t;

STD_ReturnType Timer_Init(const Timer_t *timerConfig);
STD_ReturnType Timer_Start(const Timer_t *timerConfig);
STD_ReturnType Timer_Start_IT(const Timer_t *timerConfig);
STD_ReturnType Timer_Start_PWM(const Timer_t *timerConfig);
STD_ReturnType Timer_PWM_SetDutyCycle(const Timer_t *timerConfig, uint8_t dutyCycle);
STD_ReturnType Timer_Stop_IT(const Timer_t *timerConfig);
STD_ReturnType Timer_Stop(const Timer_t *timerConfig);
STD_ReturnType Timer_GetCounter(const Timer_t *timerConfig, uint32_t* counterValue);
STD_ReturnType Timer_SetAutoReload(const Timer_t *timerConfig);
STD_ReturnType Timer_SetPrescaler(const Timer_t *timerConfig);
STD_ReturnType Timer_SetMode(const Timer_t *timerConfig);

#endif // TIMER_H
#include "interface/MCAL/rcc.h"
#include "interface/HAL/led.h"
#include "interface/HAL/switch.h"
#include "interface/HAL/sevenseg.h"
#include "interface/Core/nvic.h"
#include "interface/Core/systick.h"
#include "interface/HAL/clcd.h"
#include "OS/scheduler.h"
#include "interface/HAL/led_matrix.h"
#include "interface/MCAL/uart.h"
#include "interface/MCAL/dma.h"
#include "interface/HAL/hserial.h"
#include "interface/MCAL/spi.h"
#include "interface/HAL/hspi.h"
#include "interface/MCAL/adc.h"
#include "interface/MCAL/flash.h"
#include "interface/MCAL/timer.h"

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

void Timer_IC_Callback(void){
    LED_Toggle(LED_0);
}

// Timer_2 Channel 2 => PA1
Timer_t timer2_config = {
    .instance = TIMER_2,
    .mode = TIMER_MODE_UP,
    .prescaler = 83, // 1 us tick       => 84 MHz / (83 + 1) = 1 MHz timer clock
    .autoReloadValue = 1000 - 1, // Auto-reload value => 1 MHz / 1000 = 1 kHz timer update frequency
    .channel = TIMER_CHANNEL_2,
    .port = GPIO_PORTA,
    .pin = GPIO_PIN_1,
    .pullType = GPIO_PULLDOWN,
    .icFilter = TIMER_IC_FILTER_CONSCUTIVE_4,
    .icPolarity = TIMER_IC_POLARITY_RISING,
    .ccCallback = Timer_IC_Callback
};

int main(){
    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();

    ret = Timer_Init(&timer2_config);
    ret = Timer_Start_IC(&timer2_config);

    // uint32_t counter = 0;
    *((uint32_t*)0xE0042008) |= 0xF; // Enable debug halt for TIMER2, TIMER3, TIMER4, and TIMER5

    while(1){
        // Just Connect a switch to PA1 and observe the LED toggling on pressed(PULL DOWN) or released(PULL UP)
    }
    
    return 0;
}
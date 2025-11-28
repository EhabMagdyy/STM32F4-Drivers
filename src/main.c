#include "interface/MCAL/rcc.h"
#include "interface/HAL/led.h"
#include "interface/HAL/switch.h"
#include "interface/HAL/sevenseg.h"
#include "interface/Core/nvic.h"
#include "interface/Core/systick.h"

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

RCC_CFG_t rcc_hsi = {
    .sysClkSource = RCC_CLOCK_SOURCE_HSI,
};

RCC_CFG_t rcc_hse = {
    .sysClkSource = RCC_CLOCK_SOURCE_HSE,
};

void func(void){
    static uint8_t counter = 0, state = SEVEN_SEGMENT_DOT_OFF;
    SevenSegment_Write(SEVEN_SEGMENT_0, counter%10);
    counter++;
    SYSTICK_DelayMS(100);
    if(counter == 10){
        counter = 0;
        state = !state;
        // SevenSegment_SetDotState(SEVEN_SEGMENT_0, state);
        SevenSegment_Clear(SEVEN_SEGMENT_0);
        SYSTICK_DelayMS(500);
    }
}

int main(){    
    STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    if (ret == STD_SUCCESS) {
        ret = SevenSegment_Init();
    }

    //SYSTICK_PeriodicInterval(200, func);
    uint8_t counter = 0, state = SEVEN_SEGMENT_DOT_OFF;

    while(1){
        SevenSegment_Write(SEVEN_SEGMENT_0, counter++);
        SYSTICK_DelayMS(1000);
        if(counter == 10){
            counter = 0;
            state = !state;
            // SevenSegment_SetDotState(SEVEN_SEGMENT_0, state);
            SevenSegment_Clear(SEVEN_SEGMENT_0);
            SYSTICK_DelayMS(500);
        }
    }
    
    return 0;
}
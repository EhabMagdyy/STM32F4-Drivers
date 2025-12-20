#include "interface/MCAL/rcc.h"
#include "interface/MCAL/gpio.h"
#include "../lib/BIT_Math.h"
#include "interface/HAL/led.h"
#include "interface/HAL/switch.h"
#include "interface/HAL/sevenseg.h"

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

int main(){    
    STD_ReturnType ret = STD_SUCCESS;

    ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);

    if (ret == STD_SUCCESS) {
        ret = SevenSegment_Init();
    }

    uint8_t counter = 0, state = SEVEN_SEGMENT_DOT_OFF;

    while(1){
        SevenSegment_Write(SEVEN_SEGMENT_0, counter++);
        for(volatile uint32_t delay = 0; delay < 250000; delay++);
        if(counter == 10){
            counter = 0;
            state = !state;
            // SevenSegment_SetDotState(SEVEN_SEGMENT_0, state);
            SevenSegment_Clear(SEVEN_SEGMENT_0);
            for(volatile uint32_t delay = 0; delay < 1000000; delay++);
        }
    }

    
    return 0;
}



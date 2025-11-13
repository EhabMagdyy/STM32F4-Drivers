#include "/home/ehab/Documents/ITI_9Months/ARM/STM32F401_Drivers/include/interface/MCAL/rcc.h"
#include "/home/ehab/Documents/ITI_9Months/ARM/STM32F401_Drivers/include/interface/MCAL/gpio.h"
#include "/home/ehab/Documents/ITI_9Months/ARM/STM32F401_Drivers/lib/BIT_Math.h"
#include "stm32f401xc.h"

RCC_CFG_t rcc_cfg = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_custom_t = RCC_PLL_MAX
};

RCC_CFG_t rcc_cfg_hsi = {
    .sysClkSource = RCC_CLOCK_SOURCE_HSI,
};

GPIO_t led_A0 = {
    .port = GPIO_PORTA,
    .pin = GPIO_PIN_0,
    .mode = GPIO_MODE_OUTPUT,
    .outputType = GPIO_OUTPUT_PUSHPULL,
    .speed = GPIO_SPEED_MEDIUM,
    .pullType = GPIO_NOPULL,
    .altFunc = GPIO_AF0_SYSTEM
};

GPIO_t led_C13 = {
    .port = GPIO_PORTC,
    .pin = GPIO_PIN_13,
    .mode = GPIO_MODE_OUTPUT,
    .outputType = GPIO_OUTPUT_PUSHPULL,
    .speed = GPIO_SPEED_MEDIUM,
    .pullType = GPIO_NOPULL,
    .altFunc = GPIO_AF0_SYSTEM
};

int main(){    
    STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_cfgClk(&rcc_cfg);

    ret = RCC_ctrlPeripheral(RCC_GPIOA | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);

    ret = GPIO_Init(&led_A0);
    ret = GPIO_Init(&led_C13);

    while(1)
    {
        ret = GPIO_TogglePin(&led_A0);
        ret = GPIO_TogglePin(&led_C13);
        for(volatile uint32_t i = 0; i < 1000000; i++);
    }

    
    return 0;
}
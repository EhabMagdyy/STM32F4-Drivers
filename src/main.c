#include "/home/ehab/Documents/ITI_9Months/ARM/STM32F401_Drivers/include/interface/MCAL/RCC/rcc.h"
#include "stm32f401xc.h"
#include "../lib/BIT_Math.h"

RCC_CFG_t rcc_cfg = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_custom_t = RCC_PLL_MAX
};

RCC_CFG_t rcc_cfg_hsi = {
    .sysClkSource = RCC_CLOCK_SOURCE_HSI,
};

int main(){    
    STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_cfgClk(&rcc_cfg);

    ret = RCC_ctrlPeripheral(RCC_GPIOC | RCC_DMA1 | RCC_GPIOA, RCC_PERIPHERAL_ENABLE);

    // output PC13
    SET_BIT(GPIOC->MODER, 26);
    CLR_BIT(GPIOC->MODER, 27);

    while(1)
    {
        TOG_BIT(GPIOC->ODR, 13);
        for(volatile uint32_t i = 0; i < 1000000; i++);
    }

    
    return 0;
}
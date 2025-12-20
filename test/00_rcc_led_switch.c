#include "interface/MCAL/rcc.h"
#include "interface/MCAL/gpio.h"
#include "../lib/BIT_Math.h"
#include "interface/HAL/led.h"
#include "interface/HAL/switch.h"

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

    ret = LED_Init();
    ret = SWITCH_Init();


    Switch_State_t switchState;

    uint32_t counter = 0;

    while(1){
        // SWITCH Driver (Read Switch State)
        ret = SWITCH_ReadState(SWITCH_0, &switchState);
        
        if(ret == STD_SUCCESS){
            if(switchState == SWITCH_PRESSED){
                // LED Driver (Toggle LEDs in sequence)
                for(volatile uint8_t i = 0; i < LED_LEN; i++){
                    ret = LED_SetState(i, LED_HIGH);
                    for(volatile uint32_t j = 0; j < 100000; j++);
                    ret = LED_SetState(i, LED_LOW);
                }
                for(volatile int8_t i = LED_LEN-2; i >= 1; i--){
                    ret = LED_SetState(i, LED_HIGH);
                    for(volatile uint32_t j = 0; j < 100000; j++);
                    ret = LED_SetState(i, LED_LOW);
                }
            }
            else{
                // Do nothing
            }
        }
        else{
            // Do nothing
        }
        // Switching Clock Source during runtime (HSI (16MHz) -> HSE (25MHz) -> PLL (84MHz) -> HSI (16MHz) ...)
        counter++;
        if(counter == 1){
            ret = RCC_ConfigureClock(&rcc_hse);
        }
        else if(counter == 4){
            ret = RCC_ConfigureClock(&rcc_pll);
        }
        else if(counter == 12){
            counter = 0;
            ret = RCC_ConfigureClock(&rcc_hsi);
        }
        else{
            // Do nothing
        }
    }

    
    return 0;
}
#include "interface/MCAL/rcc.h"
#include "interface/MCAL/gpio.h"
#include "/home/ehab/Documents/ITI_9Months/ARM/STM32F401_Drivers/lib/BIT_Math.h"
#include "stm32f401xc.h"
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

    ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);

    //ret = GPIO_Init(&led_A0);
    //ret = GPIO_Init(&led_C13);

    ret = LED_Init();
    ret = SWITCH_Init();


    SWITCH_STATE_t switchState;

    // uint32_t counter = 0;

    while(1)
    {
        // SWITCH Driver
        ret = SWITCH_ReadState(SWITCH_0, &switchState);
        
        if(ret == STD_SUCCESS){
            if(switchState == SWITCH_PRESSED){
                // LED Driver
                for(volatile uint8_t i = 0; i < LED_LEN; i++){
                    ret = LED_SetState(i, LED_HIGH);
                    for(volatile uint32_t j = 0; j < 50000; j++);
                    ret = LED_SetState(i, LED_LOW);
                }
                for(volatile int8_t i = LED_LEN-2; i >= 1; i--){
                    ret = LED_SetState(i, LED_HIGH);
                    for(volatile uint32_t j = 0; j < 50000; j++);
                    ret = LED_SetState(i, LED_LOW);
                }
            }
            else{
                
            }
        }
        /*
        //ret = GPIO_TogglePin(&led_A0);
        //ret = GPIO_TogglePin(&led_C13);
        for(volatile uint32_t i = 0; i < 1000000; i++);
        counter++;
        if(counter == 6){
            ret =   RCC_ConfigureClock(&rcc_hse);
        }
        else if(counter == 14){
            ret = RCC_ConfigureClock(&rcc_pll);
        }
        else if(counter == 28){
            counter = 0;
            ret = RCC_ConfigureClock(&rcc_hsi);
        }
        */

        
    }

    
    return 0;
}
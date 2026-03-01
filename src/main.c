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

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

ADC_t adc3 = {
    .channel = ADC_CHANNEL_3,
    .resolution = ADC_RESOLUTION_12BIT,
    .sampleTime = ADC_SAMPLETIME_84CYCLES,
    .seqLength = ADC_SEQ_LENGTH_1
};

int main(){
    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();
    ret = ADC_Init(&adc3);

    uint16_t adcValue = 0;

    while(1){
        ADC_SingleRead(&adc3, &adcValue);
        if(adcValue > 3200){
            LED_SetState(LED_0, LED_HIGH);
            LED_SetState(LED_1, LED_HIGH);
        }
        else if (adcValue <= 800){
            LED_SetState(LED_0, LED_LOW);
            LED_SetState(LED_1, LED_LOW);
        }
        else if (adcValue <= 1600){
            LED_SetState(LED_0, LED_HIGH);
            LED_SetState(LED_1, LED_LOW);
        }
        else if (adcValue <= 2400){
            LED_SetState(LED_0, LED_LOW);
            LED_SetState(LED_1, LED_HIGH);
        }
        
        SYSTICK_DelayMS(1000);
    }
    
    return 0;
}
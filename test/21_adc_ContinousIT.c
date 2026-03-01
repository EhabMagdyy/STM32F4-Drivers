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

uint16_t mainADCBuffer[10] = {0};

void ADC_Callback(void){
    volatile uint16_t avg = (mainADCBuffer[0] + mainADCBuffer[1] + mainADCBuffer[2] + mainADCBuffer[3]) / 4;
    if(avg > 3000){
        LED_SetState(LED_0, LED_HIGH);
        LED_SetState(LED_1, LED_HIGH);
    }
    else if(avg > 2000){
        LED_SetState(LED_0, LED_LOW);
        LED_SetState(LED_1, LED_HIGH);
    }
    else if(avg > 1000){
        LED_SetState(LED_0, LED_HIGH);
        LED_SetState(LED_1, LED_LOW);
    }
    else if(avg <= 1000){
        LED_SetState(LED_0, LED_LOW);
        LED_SetState(LED_1, LED_LOW);
    }
}

ADC_t adc345 = {
    .channels = (ADC_Channel_t[]){ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5},
    .numChannels = 3,   
    .resolution = ADC_RESOLUTION_12BIT,
    .sampleTime = ADC_SAMPLETIME_84CYCLES,
    .callback = ADC_Callback
};

int main(){
    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();
    ret = ADC_Init(&adc345);

    while(1){
        ADC_ContinousReadIT(adc345.channels[0], mainADCBuffer, 4);
        SYSTICK_DelayMS(100);
    }
    
    return 0;
}
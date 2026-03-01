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

ADC_t adc345 = {
    .channels = (ADC_Channel_t[]){ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5},
    .numChannels = 3,   
    .resolution = ADC_RESOLUTION_12BIT,
    .sampleTime = ADC_SAMPLETIME_84CYCLES,
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

    uint16_t adcValue[10] = {0};

    while(1){
        ADC_ScanModeRead(adc345.channels, ADC_SEQ_LENGTH_3, &adcValue[0], 3);
        SYSTICK_DelayMS(1000);
    }
    
    return 0;
}
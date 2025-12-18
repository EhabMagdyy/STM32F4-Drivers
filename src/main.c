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

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

uint8_t d1[5] = "Ehab"; 
uint8_t d2[5] = "Lena"; 

void mydmaCallback(void){
    if(d2[2] == 'a'){
        LED_Toggle(LED_0);
    }
}

DMA_Instance_t dmaInstance = {
    .dmaNum = DMA_2,
    .stream = DMA_STREAM_7,
    .channel = DMA_CHANNEL_0,
    .direction = DMA_MEMORY_TO_MEMORY,
    .memInc = DMA_MEM_INC_ENABLE,
    .periphInc = DMA_PERIPH_INC_ENABLE,
    .priority = DMA_PRIORITY_LOW,
    .memBurst = DMA_MEM_BURST_SINGLE,
    .periphBurst = DMA_PERIPH_BURST_SINGLE,
    .memSize = DMA_MEM_SIZE_8BIT,
    .periphSize = DMA_PERIPH_SIZE_8BIT,
    .interruptConf = DMA_IT_COMPLETE | DMA_IT_HALF_COMPLETE | DMA_IT_DIRECT_MODE_ERROR,
    .combleteCallback = mydmaCallback,
    .halfCallback = NULL,
    .errorCallback = NULL,
    .directErrorCallback = NULL
};

int main(){

    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();
    ret = DMA_Init(&dmaInstance);

    while(1){
        ret = DMA_Start(&dmaInstance, d1, d2, 4);
        SYSTICK_DelayMS(1000);
    }
    
    return 0;
}
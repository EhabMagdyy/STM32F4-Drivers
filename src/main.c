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

DMA_Instance_t dmaInstance = {
    .dmaNum = DMA_2,
    .stream = DMA_STREAM_0,
    .channel = DMA_CHANNEL_0,
    .direction = DMA_MEMORY_TO_MEMORY,
    .memInc = DMA_MEM_INC_ENABLE,
    .periphInc = DMA_PERIPH_INC_ENABLE,
    .priority = DMA_PRIORITY_LOW,
    .memBurst = DMA_MEM_BURST_SINGLE,
    .periphBurst = DMA_PERIPH_BURST_SINGLE
};

uint8_t d1[5] = "Ehab"; 
uint8_t d2[5] = "Lena"; 

int main(){

    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();
    ret = DMA_Init(&dmaInstance);
    
    ret = DMA_Start(&dmaInstance, d1, d2, 4);

    while(1){

    }
    
    return 0;
}
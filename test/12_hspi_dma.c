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

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

uint8_t d1[12] = "My SPI Works"; 
uint8_t d2[12] = {0};

void spiCallback(void){
    if(d2[0] == 'M'){
        LED_Toggle(LED_0);
    }
    if(d2[11] == 's'){
        LED_Toggle(LED_1);
    }
}

SPI_Config_t spi1 = {
    .spiNum = SPI_1,
    .mode = SPI_MODE_MASTER,
    .dataFrame = SPI_DATA_FRAME_8BIT,
    .baudRatePrescaler = SPI_BAUDRATE_PRESCALER_4,
    .clockPhase = SPI_CLOCK_PHASE_1EDGE,
    .clockPolarity = SPI_CLOCK_POLARITY_LOW,
    .direction = SPI_DIRECTION_2LINES,
    .frameFormat = SPI_FRAME_FORMAT_MSB_FIRST,
    .frameFormatStandard = SPI_FRAME_FORMAT_MOTOROLA,
    .trancieveCallback = NULL,
    .dmaEnable = SPI_DMA_ENABLE
};

DMA_Instance_t dmaInstanceTx = {
    .dmaNum = DMA_2,
    .stream = DMA_STREAM_3,
    .channel = DMA_CHANNEL_3,
    .direction = DMA_MEMORY_TO_PERIPHERAL,
    .memInc = DMA_MEM_INC_ENABLE,
    .periphInc = DMA_PERIPH_INC_DISABLE,
    .priority = DMA_PRIORITY_LOW,
    .memBurst = DMA_MEM_BURST_SINGLE,
    .periphBurst = DMA_PERIPH_BURST_SINGLE,
    .memSize = DMA_MEM_SIZE_8BIT,
    .periphSize = DMA_PERIPH_SIZE_8BIT,
    .interruptConf = DMA_IT_COMPLETE,
    .combleteCallback = NULL,
    .halfCallback = NULL,
    .errorCallback = NULL,
    .directErrorCallback = NULL
};

DMA_Instance_t dmaInstanceRx = {
    .dmaNum = DMA_2,
    .stream = DMA_STREAM_2,
    .channel = DMA_CHANNEL_3,
    .direction = DMA_PERIPHERAL_TO_MEMORY,
    .memInc = DMA_MEM_INC_ENABLE,
    .periphInc = DMA_PERIPH_INC_DISABLE,
    .priority = DMA_PRIORITY_LOW,
    .memBurst = DMA_MEM_BURST_SINGLE,
    .periphBurst = DMA_PERIPH_BURST_SINGLE,
    .memSize = DMA_MEM_SIZE_8BIT,
    .periphSize = DMA_PERIPH_SIZE_8BIT,
    .interruptConf = DMA_IT_COMPLETE,
    .combleteCallback = spiCallback,
    .halfCallback = NULL,
    .errorCallback = NULL,
    .directErrorCallback = NULL
};

SPI_Buffer_t buffer = {
    .txData = d1,
    .rxData = d2,
    .length = 12
};

HSPI_DMA_t hspi1_dma = {
    .spiConfig = &spi1,
    .txChannel = &dmaInstanceTx,
    .rxChannel = &dmaInstanceRx
};

int main(){
    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();
    ret = HSPI_Init(&hspi1_dma, SYSTICK_CLOCK_SOURCE_PLL_MAX);

    while(1){
        ret = HSPI_StartTranceiveDMA(&hspi1_dma, &buffer);
        SYSTICK_DelayMS(100);
    }
    
    return 0;
}
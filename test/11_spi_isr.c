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
    .dataFrame = SPI_DATA_FRAME_16BIT,
    .baudRatePrescaler = SPI_BAUDRATE_PRESCALER_4,
    .clockPhase = SPI_CLOCK_PHASE_1EDGE,
    .clockPolarity = SPI_CLOCK_POLARITY_LOW,
    .direction = SPI_DIRECTION_2LINES,
    .frameFormat = SPI_FRAME_FORMAT_MSB_FIRST,
    .frameFormatStandard = SPI_FRAME_FORMAT_MOTOROLA,
    .trancieveCallback = spiCallback
};

SPI_Buffer_t buffer = {
    .txData = d1,
    .rxData = d2,
    .length = 12
};

int main(){
    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();
    ret = SPI_Init(&spi1, SYSTICK_CLOCK_SOURCE_PLL_MAX);

    while(1){
        ret = SPI_TranceiveIT(&spi1, &buffer);
        SYSTICK_DelayMS(1000);
    }
    
    return 0;
}
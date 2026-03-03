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
#include "interface/MCAL/flash.h"

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

int main(){
    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();

    uint32_t flashData[4] = {0};    
    uint32_t dataToWrite[] = {0x1, 0x2, 0x3, 0x4};
    ret = FLASH_Erase(FLASH_SECTOR_NUMBER_3);
    ret = FLASH_Write(FLASH_SECTOR_3, dataToWrite, 4);
    ret = FLASH_Read(FLASH_SECTOR_3, flashData, 4);

    while(1){
        
    }
    
    return 0;
}
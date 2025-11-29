#include "interface/MCAL/rcc.h"
#include "interface/HAL/led.h"
#include "interface/HAL/switch.h"
#include "interface/HAL/sevenseg.h"
#include "interface/Core/nvic.h"
#include "interface/Core/systick.h"
#include "interface/HAL/clcd.h"

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

void sevSegDisplay(void){
    static uint8_t counter = 0, state = SEVEN_SEGMENT_DOT_OFF;
    SevenSegment_Write(SEVEN_SEGMENT_0, counter%10);
    counter++;
    if(counter == 11){
        counter = 0;
        state = !state;
        SevenSegment_Clear(SEVEN_SEGMENT_0);
    }
}

int main(){    
    STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);

    //ret = SevenSegment_Init();

    CLCD_Init();
    CLCD_WriteCommand(CLCD_0, LCD_CLEAR);
    SYSTICK_DelayMS(2);
    CLCD_WriteCommand(CLCD_0, LCD_CURSOR_HOME);
    SYSTICK_DelayMS(2);
    CLCD_WriteStringPos(CLCD_0, 1, 4, "Ehab");
    CLCD_WriteCustomCharacter(CLCD_0, 1, 9, (uint8_t[]){0x00,0x11,0x0E,0x15,0x15,0x15,0x15,0x04}, 0);
    CLCD_WriteStringPos(CLCD_0, 1, 11, "ES46");


    //SYSTICK_PeriodicInterval(250, sevSegDisplay);

    while(1){
    }
    
    return 0;
}
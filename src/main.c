#include "interface/MCAL/rcc.h"
#include "interface/HAL/led.h"
#include "interface/HAL/switch.h"
#include "interface/HAL/sevenseg.h"
#include "interface/Core/nvic.h"
#include "interface/Core/systick.h"
#include "interface/HAL/clcd.h"
#include "OS/scheduler.h"
#include "interface/HAL/led_matrix.h"

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

void CLCD_WriteRunnable(void* arg){
    static volatile uint8_t toggle = 0;
    if(toggle == 0){
        CLCD_asyncWriteStringPos(CLCD_0, 1, 4, "Ehab");
        toggle = 1;
    }
    else if(toggle == 1){
        CLCD_syncWriteCustomCharacter(CLCD_0, 1, 9, 0);
        toggle = 2;
    }
    else if(toggle == 2){
        CLCD_asyncWriteStringPos(CLCD_0, 1, 11, "ES46");
        toggle = 3;
    }
    else{
        CLCD_asyncWriteCommand(CLCD_0, LCD_CLEAR);
        toggle = 0;
    }
}

Runnable__t lcd_writer = {
    .callback = CLCD_WriteRunnable,
    .Periodicity = 333,     // 3*333 = 1 second
    .FirstDelay = 50,
    .arg = 0
};

void LED_Runnable(void* arg){
    static uint8_t ledState = 0;
    if(ledState == 0){
        LED_SetState(LED_0, LED_HIGH);
        ledState = 1;
    }
    else{
        LED_SetState(LED_0, LED_LOW);
        ledState = 0;
    }
}

Runnable__t led_toggle = {
    .callback = LED_Runnable,
    .Periodicity = 333,         // 3*333 = 1 second
    .FirstDelay = 50,
    .arg = 0
};

void SevSeg_Runnable(void* arg){
    static uint8_t counter = 0;
    if(counter < 10){
        SevenSegment_Write(SEVEN_SEGMENT_0, counter++);
    }
    else{
        counter = 0;
        SevenSegment_Clear(SEVEN_SEGMENT_0);
    }
}

Runnable__t sevseg = {
    .callback = SevSeg_Runnable,
    .Periodicity = 333,         // 3*333 = 1 second
    .FirstDelay = 50,
    .arg = 0
};

int main(){    
    STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    
    ret = CLCD_asyncInit();
    ret = LED_Init();
    ret = SevenSegment_Init();

    ret = Scheduler_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX, 3);
    ret = Scheduler_RegisterRunnable(&lcd_writer);
    ret = Scheduler_RegisterRunnable(&led_toggle);
    ret = Scheduler_RegisterRunnable(&sevseg);
    ret = Scheduler_Start();


    while(1){
    }
    
    return 0;
}
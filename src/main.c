#include "interface/MCAL/rcc.h"
#include "interface/HAL/led.h"
#include "interface/HAL/switch.h"
#include "interface/HAL/sevenseg.h"
#include "interface/Core/nvic.h"
#include "interface/Core/systick.h"
#include "interface/HAL/clcd.h"
#include "OS/scheduler.h"

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

void ToggleLED(void* arg){
    static volatile uint8_t ledState[LED_LEN] = {LED_LOW};
    static Switch_State_t SwitchState = SWITCH_RELEASED;
    SWITCH_ReadState(SWITCH_0, &SwitchState);
    if(SwitchState == SWITCH_PRESSED){
        ledState[*(uint8_t*)arg] = !ledState[*(uint8_t*)arg];
        uint8_t ledNumber = *(uint8_t*)arg;
        LED_SetState(ledNumber, ledState[ledNumber]);
    }
}

Runnable__t runnable1 = {
    .callback = ToggleLED,
    .Periodicity = 100,
    .FirstDelay = 5,
    .arg = LED_0
};

Runnable__t runnable2 = {
    .callback = ToggleLED,
    .Periodicity = 140,
    .FirstDelay = 0,
    .arg = LED_1
};

Runnable__t runnable3 = {
    .callback = ToggleLED,
    .Periodicity = 70,
    .FirstDelay = 0,
    .arg = LED_2
};

int main(){    
    STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = LED_Init();
    ret = SWITCH_Init();

    ret = Scheduler_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX, 1);
    ret = Scheduler_RegisterRunnable(&runnable1);
    ret = Scheduler_RegisterRunnable(&runnable2);
    ret = Scheduler_RegisterRunnable(&runnable3);
    Scheduler_Start();

    while(1){
        for(volatile uint32_t i = 0; i < 500000; i++);
    }
    
    return 0;
}
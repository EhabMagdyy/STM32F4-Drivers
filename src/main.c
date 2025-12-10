#include "interface/MCAL/rcc.h"
#include "interface/HAL/led.h"
#include "interface/HAL/switch.h"
#include "interface/HAL/sevenseg.h"
#include "interface/Core/nvic.h"
#include "interface/Core/systick.h"
#include "interface/HAL/clcd.h"
#include "OS/scheduler.h"
#include "interface/HAL/led_matrix.h"
#include <stdlib.h>

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

// Stickman frame 0 (standing)
uint8_t man_frame0[8] = {
    0x04,
    0x0E,
    0x04,
    0x0A,
    0x11,
    0x04,
    0x0A,
    0x11
};

// Stickman frame 1 (walking)
uint8_t man_frame1[8] = {
    0x04,
    0x0E,
    0x04,
    0x0A,
    0x04,
    0x11,
    0x04,
    0x0A
};

uint8_t tree[8] = {
  0x0E,
  0x1F,
  0x1F,
  0x1F,
  0x0E,
  0x04,
  0x04,
  0x00
};

int x = 9;

void CLCD_WriteRunnable(void* arg){
    CLCD_Instance_t lcdID = *(CLCD_Instance_t*)arg;
    static volatile uint8_t state = 0;
    static volatile uint8_t col = 1;
    static volatile uint8_t row = 2;

    switch(state){
        case 0:
            CLCD_asyncSaveCustomCharacter(lcdID, man_frame0, 0);
            state = 1;
            break;
        case 1:
            CLCD_asyncSaveCustomCharacter(lcdID, man_frame1, 1);
            state = 2;
            break;
        case 2:
            CLCD_asyncSaveCustomCharacter(lcdID, tree, 2);
            state = 3;
            break;
        case 3: 
            CLCD_asyncWriteCustomCharacter(lcdID, 2, x, 2);
            state = 4;
            break;
        case 4:
            // Clear previous position
            CLCD_asyncWriteStringPos(lcdID, row, col, " ");
            if(col < 16){
                col++;
            }
            else{
                x = 3 + rand() % (13 - 3 + 1);
                col = 1;
                state = 7;
                break;
            }
            if(col == x) row = 1;
            else row = 2;
            state = 5;
            break;
        case 5: CLCD_asyncWriteCustomCharacter(lcdID, row, col, 0);
            state = 6;
            break;
        case 6: CLCD_asyncWriteCustomCharacter(lcdID, row, col, 1);
            state = 3;
            break;
        case 7:
            CLCD_asyncWriteCommand(lcdID, LCD_CLEAR);
            state = 3;
            break;
        default: state = 2; break;
    }
}

Runnable__t lcd_writer = {
    .callback = CLCD_WriteRunnable,
    .Periodicity = 50,
    .FirstDelay = 150,
    .arg = 0
};

int main(){    
    STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);

    ret = CLCD_asyncInit();

    ret = Scheduler_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX, 1);
    ret = Scheduler_RegisterRunnable(&lcd_writer);
    ret = Scheduler_Start();


    while(1){}
    
    return 0;
}
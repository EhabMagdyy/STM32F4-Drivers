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

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

uint8_t TxBuffer[4] = "Ack\0";

Buffer_t txBuf = { 
    .data = TxBuffer, 
    .length = 4,
    .index = 0
};

uint8_t RxBuffer[50] = {0};

Buffer_t rxBuf = { 
    .data = RxBuffer, 
    .length = 4,
    .index = 0
};

void UART_RxCallback(void);

UART_Config_t uart1_config = {
    .UartInstance = UART2,
    .BaudRate = UART_BAUDRATE_115200,
    .DataBits = UART_DATABITS_8,
    .Parity = UART_PARITY_NONE,
    .port = GPIO_PORTA,
    .txPin = GPIO_PIN_2,
    .txCallback = NULL,
    .rxCallback = UART_RxCallback
};

volatile uint8_t flag = 0;

void UART_RxCallback(void){
    if(rxBuf.data[0] == 'A'){
        flag = 1;
    }
}

int main(){

    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = UART_Init(&uart1_config, SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();

    ret = UART_SendBufferIT(&uart1_config, &txBuf);

    while(1){
        ret = UART_ReceiveBufferIT(&uart1_config, &rxBuf);
        SYSTICK_DelayMS(1000);
        if(flag == 1){
            ret = UART_SendBufferIT(&uart1_config, &txBuf);
            ret = LED_Toggle(LED_0);
            flag = 0;
        }
    }
    
    return 0;
}
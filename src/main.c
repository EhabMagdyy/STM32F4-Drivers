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

GPIO_t uart1_tx_pin = {
    .port       = GPIO_PORTA,
    .pin        = GPIO_PIN_9,
    .mode       = GPIO_MODE_AF,
    .outputType = GPIO_OUTPUT_PUSHPULL,
    .speed      = GPIO_SPEED_HIGH,
    .pullType   = GPIO_NOPULL,
    .altFunc    = GPIO_AF7_USART1_2
};

GPIO_t uart1_rx_pin = {
    .port       = GPIO_PORTA,
    .pin        = GPIO_PIN_10,
    .mode       = GPIO_MODE_AF,
    .outputType = GPIO_OUTPUT_PUSHPULL,
    .speed      = GPIO_SPEED_HIGH,
    .pullType   = GPIO_NOPULL,
    .altFunc    = GPIO_AF7_USART1_2
};

volatile uint8_t data[10] = {0};

void UART_RxCallback(void);

UART_Config_t uart1_config = {
    .UartInstance = UART1,
    .BaudRate = UART_BAUDRATE_115200,
    .DataBits = UART_DATABITS_8,
    .Parity = UART_PARITY_NONE,
    .txCallback = NULL,
    .rxCallback = UART_RxCallback
};

void UART_RxCallback(void){
    if(data[0] == 'D'){
        UART_SendBufferIT(&uart1_config, (uint8_t*)"Ack\0", 4);
    }
}

int main(){

    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
        ret = RCC_ControlPeripheral(RCC_USART1 | RCC_USART6, RCC_PERIPHERAL_ENABLE);
    }
    
    ret = NVIC_EnableIRQ(USART1_IRQn);

    ret = GPIO_Init(&uart1_tx_pin);
    ret = GPIO_Init(&uart1_rx_pin);

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = UART_Init(&uart1_config, SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();

    while(1){
        ret = UART_SendBufferIT(&uart1_config, (uint8_t*)"Ehab\0", 5);
        ret = UART_ReceiveBufferIT(&uart1_config, (uint8_t*)&data, 7);
        SYSTICK_DelayMS(1000);
    }
    
    return 0;
}
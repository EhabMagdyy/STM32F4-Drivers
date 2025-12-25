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

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

uint8_t d1[12] = "Ehab1234567"; 
uint8_t d2[12] = {0}; 

void myTxCallback(void){
    LED_Toggle(LED_0);
}

void myRxCallback(void){
    LED_Toggle(LED_1);
}

UART_Config_t uart1_config = {
    .UartInstance = UART1,
    .BaudRate = UART_BAUDRATE_115200,
    .DataBits = UART_DATABITS_8,
    .Parity = UART_PARITY_NONE,
    .port = GPIO_PORTA,
    .txPin = GPIO_PIN_9,
    .txCallback = myTxCallback,
    .rxCallback = myRxCallback,
    .dmaEnable = UART_DMA_DISABLE
};

HSerial_Buffer_t hserial_txBuffer = {
    .buffer.data = d1,
    .buffer.index = 0,
    .buffer.length = 12
};

HSerial_Buffer_t hserial_rxBuffer = {
    .buffer.data = d2,
    .buffer.index = 0,
    .buffer.length = 12
};

HSerial_Config_t hserialConfig = {
    .uartConfig = &uart1_config,
    .txDma = NULL,
    .rxDma = NULL,
    .txBuffer = &hserial_txBuffer,
    .rxBuffer = &hserial_rxBuffer
};

int main(){
    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();

    ret = HSerial_Init(&hserialConfig, SYSTICK_CLOCK_SOURCE_PLL_MAX);

    while(1){
        ret = HSerial_ReceiveBufferIT(&hserialConfig);
        if(ret != STD_SUCCESS){
            continue;
        }
        ret = HSerial_SendBufferIT(&hserialConfig);
        if(ret != STD_SUCCESS){
            continue;
        }
        SYSTICK_DelayMS(1000);
    }
    
    return 0;
}
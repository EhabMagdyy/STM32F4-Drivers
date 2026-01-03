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
#include "interface/MCAL/i2c.h"
#include "interface/HAL/hi2c.h"
#include "interface/HAL/eeprom.h"

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

uint8_t writeData[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                             0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};

uint8_t dataAddr[16] = {0x00};

void i2c1Callback(void){
    if(dataAddr[0] == 0x01){
        LED_Toggle(LED_0);
    }
    if(dataAddr[15] == 0x10){
        LED_Toggle(LED_1);
    }
}

I2C_Config_t i2c1Config = {
    .i2cNumber = I2C_1,
    .mode = I2C_MASTER_MODE,
    .apb1ClockFreq = 42000000U, // 42 MHz
    .busSpeed = I2C_BUS_SPEED_STANDARD, // 100 kHz
    .addrMode = I2C_7BIT_ADDR_MODE,
    .receiveCallback = i2c1Callback,
    .dmaEnable = I2C_DMA_DISABLE
};

EEPROM_Config_t eepromConfig = {
    .i2cConfig = &i2c1Config,
    .deviceAddress = 0xA0
};

void EEPROM_Runnable(void* arg){
    static uint8_t counter = 0;
    if(counter == 0){
        EEPROM_WritePage(&eepromConfig, 2, writeData);
        counter = 1;
    }
    else if(counter == 1){
        EEPROM_ReadPage(&eepromConfig, 2, dataAddr);
        counter = 2;
    }
    else if(counter == 2){
        EEPROM_ReadPage(&eepromConfig, 2, dataAddr);
        counter = 1;
    }
    else{
        counter = 1;
    }
}

static Runnable__t eepromRunnable = {
    .callback = EEPROM_Runnable,
    .Periodicity = 100,
    .FirstDelay = 100,
    .arg = NULL
};

int main(){
    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = Scheduler_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX, 1);
    ret = LED_Init();
    ret = EEPROM_Init(&eepromConfig);
    ret = Scheduler_RegisterRunnable(&eepromRunnable);
    Scheduler_Start();

    while(1){
        
    }
    
    return 0;
}
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

RCC_CFG_t rcc_pll = {
    .sysClkSource = RCC_CLOCK_SOURCE_PLL,
    .pllClkSource = RCC_CLOCK_SOURCE_HSE,
    .pllConfig.pll_cfg_max_t = { .pllMax = RCC_PLL_MAX }
};

uint8_t i2c1Data[3] = "EMZ";
uint8_t i2c2Data[1] = "M";

I2C_Buffer_t i2c1Buffer = {
    .data = i2c1Data,
    .length = 3,
    .index = 0
};

void i2c1Callback(void){
    LED_Toggle(LED_0);
}

I2C_Config_t i2c1Config = {
    .i2cNumber = I2C_1,
    .mode = I2C_MASTER_MODE,
    .apb1ClockFreq = 42000000U, // 42 MHz
    .busSpeed = I2C_BUS_SPEED_STANDARD, // 100 kHz
    .addrMode = I2C_7BIT_ADDR_MODE,
    .transferCompleteCallback = i2c1Callback,
    .dmaEnable = I2C_DMA_DISABLE
};

int main(){
    volatile STD_ReturnType ret = STD_SUCCESS;
    ret = RCC_ConfigureClock(&rcc_pll);
    if(ret == STD_SUCCESS){
        ret = RCC_ControlPeripheral(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC, RCC_PERIPHERAL_ENABLE);
    }

    ret = SYSTICK_Init(SYSTICK_CLOCK_SOURCE_PLL_MAX);
    ret = LED_Init();
    ret = I2C_Init(&i2c1Config);

    while(1){
        ret = I2C_Master_TransmitIT(&i2c1Config, 0x52, &i2c1Buffer);
        SYSTICK_DelayMS(500);
        i2c1Buffer.data[0] = 'M';
        ret = I2C_Master_TransmitIT(&i2c1Config, 0x52, &i2c1Buffer);
        SYSTICK_DelayMS(500);
        i2c1Buffer.data[0] = 'E';
        ret = I2C_Master_TransmitIT(&i2c1Config, 0x52, &i2c1Buffer);
        SYSTICK_DelayMS(500);
        i2c1Buffer.data[0] = 'M';
    }
    
    return 0;
}
#include "../../../../lib/STD_Types.h"

#define RCC_PLL_MAX       0xFF

typedef enum {
    RCC_DISABLE = 0,
    RCC_ENABLE
} RCC_Clock_Status_t;

typedef enum {
    RCC_CLOCK_SOURCE_HSI = 0,
    RCC_CLOCK_SOURCE_HSE,
    RCC_CLOCK_SOURCE_PLL
} RCC_ClockType_t;

typedef enum {
    RCC_AHB1 = 0,
    RCC_AHB2,
    RCC_APB1,
    RCC_APB2
} RCC_BusType_t;

typedef enum {
    RCC_PERIPHERAL_DISABLE = 0,
    RCC_PERIPHERAL_ENABLE,
    RCC_PERIPHERAL_RESET
} RCC_Peripheral_Operation_t;

typedef union {
    struct {
        uint16_t PLLN;  // Multiplication factor for the main PLL VCO
        uint8_t PLLM;   // Division factor for the main PLL input clock
        uint8_t PLLP;   // Division factor for main system clock
        uint8_t PLLQ;   // Division factor for USB OTG FS, SDIO and RNG clocks
    } pll_cfg_custom_t;
    struct {
        uint8_t pllMax;
    } pll_cfg_max_t;
} PLL_CFG_t;

typedef struct {
    RCC_ClockType_t sysClkSource;
    RCC_ClockType_t pllClkSource;
    PLL_CFG_t pllConfig;
} RCC_CFG_t;

/*
typedef enum {
    RCC_CLK_GPIOA   = (1U << 0),  // Bit 0: GPIOA
    RCC_CLK_GPIOB   = (1U << 1),  // Bit 1: GPIOB
    RCC_CLK_GPIOC   = (1U << 2),  // Bit 2: GPIOC
    RCC_CLK_GPIOD   = (1U << 3),  // Bit 3: GPIOD
    RCC_CLK_GPIOE   = (1U << 4),  // Bit 4: GPIOE
    RCC_CLK_GPIOH   = (1U << 7),  // Bit 7: GPIOH

    RCC_CLK_CRC     = (1U << 12), // Bit 12: CRC
    RCC_CLK_DMA1    = (1U << 21), // Bit 21: DMA1
    RCC_CLK_DMA2    = (1U << 22), // Bit 22: DMA2
    RCC_CLK_OTGFS   = (1U << 25), // Bit 25: USB OTG FS (AHB1)    
} RCC_AHB1_CLK_Peripheral_t;

typedef enum {
    RCC_CLK_TIM2    = (1U << 0),  // Bit 0: TIM2
    RCC_CLK_TIM3    = (1U << 1),  // Bit 1: TIM3
    RCC_CLK_TIM4    = (1U << 2),  // Bit 2: TIM4
    RCC_CLK_TIM5    = (1U << 3),  // Bit 3: TIM5
    RCC_CLK_WWDG    = (1U << 11), // Bit 11: Window Watchdog
    RCC_CLK_SPI2    = (1U << 14), // Bit 14: SPI2
    RCC_CLK_SPI3    = (1U << 15), // Bit 15: SPI3
    RCC_CLK_USART2  = (1U << 17), // Bit 17: USART2
    RCC_CLK_I2C1    = (1U << 21), // Bit 21: I2C1
    RCC_CLK_I2C2    = (1U << 22), // Bit 22: I2C2
    RCC_CLK_I2C3    = (1U << 23), // Bit 23: I2C3
    RCC_CLK_PWR     = (1U << 28), // Bit 28: Power interface
} RCC_APB1_Peripheral_t;

typedef enum {
    RCC_CLK_TIM1    = (1U << 0),  // Bit 0: TIM1
    RCC_CLK_USART1  = (1U << 4),  // Bit 4: USART1      
    RCC_CLK_USART6  = (1U << 5),  // Bit 5: USART6
    RCC_CLK_ADC1    = (1U << 8),  // Bit 8: ADC1
    RCC_CLK_SDIO    = (1U << 11), // Bit 11: SDIO
    RCC_CLK_SPI1    = (1U << 12), // Bit 12: SPI1
    RCC_CLK_SPI4    = (1U << 13), // Bit 13: SPI4
    RCC_CLK_SYSCFG  = (1U << 14), // Bit 14: System configuration controller
    RCC_CLK_TIM9    = (1U << 16), // Bit 16: TIM9
    RCC_CLK_TIM10   = (1U << 17), // Bit 17: TIM10
    RCC_CLK_TIM11   = (1U << 18), // Bit 18: TIM11
} RCC_APB2_Peripheral_t;
*/

#define RCC_BUS_MASK        0xC0000000U 
#define RCC_BUS_OFFSET      30
#define RCC_PERIPH_MASK     0x3FFFFFFFU

#define RCC_BUS_AHB1_ID (RCC_AHB1 << RCC_BUS_OFFSET)
#define RCC_BUS_AHB2_ID (RCC_AHB2 << RCC_BUS_OFFSET)
#define RCC_BUS_APB1_ID (RCC_APB1 << RCC_BUS_OFFSET)
#define RCC_BUS_APB2_ID (RCC_APB2 << RCC_BUS_OFFSET)

typedef enum {
    // AHB1 Peripherals (Bus ID 0)
    RCC_GPIOA   = RCC_BUS_AHB1_ID | (1U << 0),
    RCC_GPIOB   = RCC_BUS_AHB1_ID | (1U << 1),
    RCC_GPIOC   = RCC_BUS_AHB1_ID | (1U << 2),
    RCC_GPIOD   = RCC_BUS_AHB1_ID | (1U << 3),
    RCC_GPIOE   = RCC_BUS_AHB1_ID | (1U << 4),
    RCC_GPIOH   = RCC_BUS_AHB1_ID | (1U << 7),
    RCC_CRC     = RCC_BUS_AHB1_ID | (1U << 12),
    RCC_DMA1    = RCC_BUS_AHB1_ID | (1U << 21),
    RCC_DMA2    = RCC_BUS_AHB1_ID | (1U << 22),

    // AHB2 Peripherals (Bus ID 1)
    RCC_OTGFS   = RCC_BUS_AHB2_ID | (1U << 7),

    // APB1 Peripherals (Bus ID 1)
    RCC_TIM2    = RCC_BUS_APB1_ID | (1U << 0),
    RCC_TIM3    = RCC_BUS_APB1_ID | (1U << 1),
    RCC_TIM4    = RCC_BUS_APB1_ID | (1U << 2),
    RCC_TIM5    = RCC_BUS_APB1_ID | (1U << 3),
    RCC_WWDG    = RCC_BUS_APB1_ID | (1U << 11),
    RCC_SPI2    = RCC_BUS_APB1_ID | (1U << 14),
    RCC_SPI3    = RCC_BUS_APB1_ID | (1U << 15),
    RCC_USART2  = RCC_BUS_APB1_ID | (1U << 17),
    RCC_I2C1    = RCC_BUS_APB1_ID | (1U << 21),
    RCC_I2C2    = RCC_BUS_APB1_ID | (1U << 22),
    RCC_I2C3    = RCC_BUS_APB1_ID | (1U << 23),
    RCC_PWR     = RCC_BUS_APB1_ID | (1U << 28),

    // APB2 Peripherals (Bus ID 2)
    RCC_TIM1    = RCC_BUS_APB2_ID | (1U << 0),
    RCC_USART1  = RCC_BUS_APB2_ID | (1U << 4),
    RCC_USART6  = RCC_BUS_APB2_ID | (1U << 5),
    RCC_ADC1    = RCC_BUS_APB2_ID | (1U << 8),
    RCC_SDIO    = RCC_BUS_APB2_ID | (1U << 11),
    RCC_SPI1    = RCC_BUS_APB2_ID | (1U << 12),
    RCC_SPI4    = RCC_BUS_APB2_ID | (1U << 13),
    RCC_SYSCFG  = RCC_BUS_APB2_ID | (1U << 14),
    RCC_TIM9    = RCC_BUS_APB2_ID | (1U << 16),
    RCC_TIM10   = RCC_BUS_APB2_ID | (1U << 17),
    RCC_TIM11   = RCC_BUS_APB2_ID | (1U << 18),
} RCC_Peripheral_t;


typedef enum {
    RCC_AHB_PRESCALER_DIV1 = 0,
    RCC_AHB_PRESCALER_DIV2 = 8,
    RCC_AHB_PRESCALER_DIV4 = 16,
    RCC_AHB_PRESCALER_DIV8 = 24,
    RCC_AHB_PRESCALER_DIV16 = 32,
    RCC_AHB_PRESCALER_DIV64 = 64,
    RCC_AHB_PRESCALER_DIV128 = 128,
    RCC_AHB_PRESCALER_DIV256 = 256,
    RCC_AHB_PRESCALER_DIV512 = 512
} RCC_AHB_Prescaler_t;

typedef enum {
    RCC_APB_PRESCALER_DIV1 = 0,
    RCC_APB_PRESCALER_DIV2 = 4,
    RCC_APB_PRESCALER_DIV4 = 5,
    RCC_APB_PRESCALER_DIV8 = 6,
    RCC_APB_PRESCALER_DIV16 = 7
} RCC_APB_Prescaler_t;


STD_ReturnType RCC_cfgClk(const RCC_CFG_t *cfg);
STD_ReturnType RCC_setSysClk(RCC_ClockType_t clockType);
STD_ReturnType RCC_setClk(RCC_ClockType_t clockType, RCC_Clock_Status_t enDis);
STD_ReturnType RCC_waitForClkReady(RCC_ClockType_t clockType, uint32_t timeout);
STD_ReturnType RCC_waitForSysClkReady(RCC_ClockType_t clockType, uint32_t timeout);

STD_ReturnType RCC_pllCfg(const PLL_CFG_t *pllCfg);
STD_ReturnType RCC_setPLLClockSource(RCC_ClockType_t source);
STD_ReturnType RCC_checkPLLClockSource(RCC_ClockType_t *source);
STD_ReturnType RCC_setPLLMaxClock(void);
/*
STD_ReturnType RCC_enableClkPeripheral(uint32_t peripheral, RCC_BusType_t bus);
STD_ReturnType RCC_disableClkPeripheral(uint32_t peripheral, RCC_BusType_t bus);
STD_ReturnType RCC_resetPeripheral(uint32_t peripheral, RCC_BusType_t bus);
*/
STD_ReturnType RCC_ctrlPeripheral(RCC_Peripheral_t peripheral, RCC_Peripheral_Operation_t operation);

STD_ReturnType RCC_setAHBPrescaler(RCC_AHB_Prescaler_t prescaler);
STD_ReturnType RCC_setAPBPrescaler(RCC_APB_Prescaler_t prescaler, RCC_BusType_t bus);

STD_ReturnType RCC_setHSEBypass(void);          // if using External clock source
STD_ReturnType RCC_clearHSEBypass(void);        // if using External crystal/ceramic resonator

STD_ReturnType RCC_clockSecurityEnable(void);
STD_ReturnType RCC_clockSecurityDisable(void);

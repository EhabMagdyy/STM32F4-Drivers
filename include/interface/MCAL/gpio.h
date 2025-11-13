#include "../../../lib/STD_Types.h"

#ifndef _GPIO_H_
#define _GPIO_H_

#define GPIO_MODER_MASK       0x3U
#define GPIO_SPEED_MASK       0x3U
#define GPIO_PULL_MASK        0x3U

typedef enum {
    GPIO_PORTA = 0,
    GPIO_PORTB,
    GPIO_PORTC,
    GPIO_PORTD,
    GPIO_PORTE
} GPIO_Port_t;

typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_ALL = 0xFFFF
} GPIO_Pin_t;

typedef enum {
    GPIO_MODE_INPUT = 0x0U,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF,
    GPIO_MODE_ANALOG
} GPIO_Mode_t;


typedef enum {
    GPIO_OUTPUT_PUSHPULL = 0x0U,
    GPIO_OUTPUT_OPENDRAIN
} GPIO_OutputType_t;

typedef enum {
    GPIO_SPEED_LOW = 0x0U,
    GPIO_SPEED_MEDIUM,
    GPIO_SPEED_HIGH,
    GPIO_SPEED_VERY_HIGH
} GPIO_Speed_t;

typedef enum {
    GPIO_NOPULL = 0x0U,
    GPIO_PULLUP,
    GPIO_PULLDOWN
} GPIO_Pull_t;

typedef enum {
    GPIO_PIN_RESET = 0x0U,
    GPIO_PIN_SET
} GPIO_PinState_t;

typedef enum {
    GPIO_AF0_SYSTEM       = 0x0U, // Default AF -> GPIO
    GPIO_AF1_TIM1_TIM2,
    GPIO_AF2_TIM3_TIM4_5,
    GPIO_AF3_TIM9_10_11,
    GPIO_AF4_I2C1_2_3,
    GPIO_AF5_SPI1_2,
    GPIO_AF6_SPI3,
    GPIO_AF7_USART1_2,
    GPIO_AF8_USART6,
    GPIO_AF9_I2C2_3,
    GPIO_AF10_OTG_FS,
    GPIO_AF12_SDIO,  
    GPIO_AF15_EVENTOUT
} GPIO_AltFunc_t;

typedef struct{
    GPIO_Port_t port;
    GPIO_Pin_t pin;
    GPIO_Mode_t mode;
    GPIO_Speed_t speed;
    GPIO_OutputType_t outputType;
    GPIO_Pull_t pullType;
    GPIO_AltFunc_t altFunc;
} GPIO_t;

STD_ReturnType GPIO_Init(GPIO_t* gpio);
STD_ReturnType GPIO_DeInit(GPIO_t* gpio);
STD_ReturnType GPIO_SetMode(GPIO_t* gpio, GPIO_Mode_t mode);
STD_ReturnType GPIO_SetSpeed(GPIO_t* gpio, GPIO_Speed_t speed);
STD_ReturnType GPIO_SetPull(GPIO_t* gpio, GPIO_Pull_t pull);
STD_ReturnType GPIO_SetOutputType(GPIO_t* gpio, GPIO_OutputType_t outputType);
STD_ReturnType GPIO_SetAltFunction(GPIO_t* gpio, GPIO_AltFunc_t altFunc);

STD_ReturnType GPIO_WritePin(GPIO_t* gpio, GPIO_PinState_t state);
STD_ReturnType GPIO_ReadPin(GPIO_t* gpio, GPIO_PinState_t* state);
STD_ReturnType GPIO_TogglePin(GPIO_t* gpio);

#endif /* _GPIO_H */
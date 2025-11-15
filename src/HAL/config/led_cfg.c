#include "configuration/HAL/led_cfg.h"
#include "interface/HAL/led.h"

#include "interface/MCAL/gpio.h"

const LED_Config_t LEDS[LED_LEN] = {
    [LED_0] = { .port = GPIO_PORTA, .pin = GPIO_PIN_0, .activeState = LED_ACTIVE_HIGH, .isPP = 1 },
    [LED_1] = { .port = GPIO_PORTA, .pin = GPIO_PIN_1, .activeState = LED_ACTIVE_HIGH, .isPP = 1 },
    [LED_2] = { .port = GPIO_PORTA, .pin = GPIO_PIN_2, .activeState = LED_ACTIVE_HIGH, .isPP = 1 },
    [LED_3] = { .port = GPIO_PORTA, .pin = GPIO_PIN_3, .activeState = LED_ACTIVE_HIGH, .isPP = 1 },
    [LED_4] = { .port = GPIO_PORTA, .pin = GPIO_PIN_4, .activeState = LED_ACTIVE_HIGH, .isPP = 1 },
    [LED_5] = { .port = GPIO_PORTA, .pin = GPIO_PIN_5, .activeState = LED_ACTIVE_HIGH, .isPP = 1 },
};
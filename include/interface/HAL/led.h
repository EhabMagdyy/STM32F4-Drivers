#ifndef LED_H
#define LED_H

#include "../../../lib/STD_Types.h"
#include "configuration/HAL/led_cfg.h"

typedef enum {
    LED_ACTIVE_LOW = 0,
    LED_ACTIVE_HIGH
} LED_ACTIVE_STATE_t;

typedef enum {
    LED_HIGH = 0,
    LED_LOW
} LED_STATE_t;

typedef struct LED_Config {
    uint8_t port        : 4;
    uint8_t pin         : 4;
    uint8_t activeState  : 1;
    uint8_t isPP        : 1;
} LED_Config_t;

STD_ReturnType LED_Init(void);
STD_ReturnType LED_SetState(uint8_t ledName, LED_STATE_t state);
STD_ReturnType LED_Toggle(uint8_t ledName);

#endif // LED_H
#include "configuration/HAL/switch_cfg.h"
#include "interface/HAL/switch.h"

#include "interface/MCAL/gpio.h"

const SWITCH_Config_t SWITCHES[SWITCH_LEN] = {
    [SWITCH_0] = { .port = GPIO_PORTB, .pin = GPIO_PIN_0, .resConfig = SWITCH_INTERNAL_PULLUP }};
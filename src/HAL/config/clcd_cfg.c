#include "configuration/HAL/clcd_cfg.h"
#include "interface/HAL/clcd.h"

CLCD_t clcd[CLCD_LEN] =
{
    [CLCD_0].rsPin.port = GPIO_PORTA,
    [CLCD_0].rsPin.pin = GPIO_PIN_0,
    [CLCD_0].rwPin.port = GPIO_PORTA,
    [CLCD_0].rwPin.pin = GPIO_PIN_1,
    [CLCD_0].enPin.port = GPIO_PORTA,
    [CLCD_0].enPin.pin = GPIO_PIN_2,
    [CLCD_0].dataPins[0].port = GPIO_PORTA,
    [CLCD_0].dataPins[0].pin = GPIO_PIN_3,
    [CLCD_0].dataPins[1].port = GPIO_PORTA,
    [CLCD_0].dataPins[1].pin = GPIO_PIN_4,
    [CLCD_0].dataPins[2].port = GPIO_PORTA,
    [CLCD_0].dataPins[2].pin = GPIO_PIN_5,
    [CLCD_0].dataPins[3].port = GPIO_PORTA,
    [CLCD_0].dataPins[3].pin = GPIO_PIN_6, 
};

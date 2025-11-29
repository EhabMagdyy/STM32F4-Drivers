#include "configuration/HAL/sevenseg_cfg.h"
#include "interface/HAL/sevenseg.h"

#include "interface/MCAL/gpio.h"

const SEVEN_SEGMENT_Config_t SEVEN_SEGMENTS[SEVEN_SEGMENT_LEN] = {
    [SEVEN_SEGMENT_0] = { .port = GPIO_PORTA,
                          .pins = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, 
                                   GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8},     // A B C D E F G DOT COM
                          .type = SEVEN_SEGMENT_COMMON_CATHODE},
};
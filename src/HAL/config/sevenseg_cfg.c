#include "configuration/HAL/sevenseg_cfg.h"
#include "interface/HAL/sevenseg.h"

#include "interface/MCAL/gpio.h"

const SEVEN_SEGMENT_Config_t SEVEN_SEGMENTS[SEVEN_SEGMENT_LEN] = {
    [SEVEN_SEGMENT_0] = { .port = GPIO_PORTA,
                          .pins = {0, 1, 2, 3, 4, 5, 6, 7},     // A B C D E F G DOT
                          .type = SEVEN_SEGMENT_COMMON_CATHODE},
};
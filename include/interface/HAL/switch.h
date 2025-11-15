#ifndef SWITCH_H
#define SWITCH_H

#include "../../../lib/STD_Types.h"
#include "configuration/HAL/switch_cfg.h"

typedef enum {
    SWITCH_INTERNAL_PULLUP = 0,
    SWITCH_EXTERNAL_PULLUP,
    SWITCH_INTERNAL_PULLDOWN,
    SWITCH_EXTERNAL_PULLDOWN,
    SWITCH_FLOATING
} SWITCH_RES_CFG_t;

typedef enum {
    SWITCH_RELEASED = 0,
    SWITCH_PRESSED
} SWITCH_STATE_t;

typedef struct SWITCH_Config {
    uint8_t port        : 4;
    uint8_t pin         : 4;
    uint8_t resConfig   : 3;
} SWITCH_Config_t;

STD_ReturnType SWITCH_Init(void);
STD_ReturnType SWITCH_ReadState(uint8_t switchName, SWITCH_STATE_t* state);

#endif // SWITCH_H
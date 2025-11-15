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
} Switch_State_t;

typedef struct SWITCH_Config {
    uint8_t port        : 4;
    uint8_t pin         : 4;
    uint8_t resConfig   : 3;
} SWITCH_Config_t;

/**
 * @brief Initializes the configured switches.
 * @retval STD_SUCCESS if initialization was successful, otherwise STD_ERROR.
 */
STD_ReturnType SWITCH_Init(void);

/**
 * @brief Reads the state of the specified switch.
 * @param switchName: The name/index of the switch to read.
 * @param state: Pointer to store the read state (SWITCH_PRESSED or SWITCH_RELEASED).
 * @retval STD_SUCCESS if read was successful, otherwise STD_ERROR.
 */
STD_ReturnType SWITCH_ReadState(uint8_t switchName, Switch_State_t* state);

#endif // SWITCH_H
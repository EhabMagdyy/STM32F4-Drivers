#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include "interface/MCAL/gpio.h"
#include "../../../lib/STD_Types.h"
#include "configuration/HAL/led_matrix_cfg.h"

typedef enum {
    LEDMATRIX_STATE_OFF,
    LEDMATRIX_STATE_ON
} LEDMATRIX_State_t;

typedef struct {
    GPIO_t rowPins[LEDMATRIX_ROWS_NUM];
    GPIO_t colPins[LEDMATRIX_COLS_NUM];
} LEDMATRIX_Cfg_t;

void LEDMATRIX_Init();
void LEDMATRIX_SetPattern(LEDMATRIX_ID_t cfg, const uint8_t pattern[LEDMATRIX_ROWS_NUM]);

#endif  /* LED_MATRIX_H */
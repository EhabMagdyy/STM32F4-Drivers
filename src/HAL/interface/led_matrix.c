#include "interface/HAL/led_matrix.h"
#include "configuration/HAL/led_matrix_cfg.h"

extern LEDMATRIX_Cfg_t ledMatrixCfg[LEDMATRIX_LEN];

void LEDMATRIX_Init() {
    for (uint8_t i = 0; i < LEDMATRIX_ROWS_NUM; i++) {
        GPIO_Init(&ledMatrixCfg[LEDMATRIX_0].rowPins[i]);
    }
    for (uint8_t j = 0; j < LEDMATRIX_COLS_NUM; j++) {
        GPIO_Init(&ledMatrixCfg[LEDMATRIX_0].colPins[j]);
    }
}

void LEDMATRIX_SetPattern(LEDMATRIX_ID_t cfg, const uint8_t pattern[8])
{
    for (uint8_t row = 0; row < 8; row++)
    {
        // turn OFF all rows (high)
        for (uint8_t i = 0; i < 8; i++)
            GPIO_WritePin(&ledMatrixCfg[cfg].rowPins[i], GPIO_PIN_SET);

        uint8_t rowData = pattern[row];

        // write columns
        for (uint8_t col = 0; col < 8; col++)
        {
            uint8_t bitIndex = 7 - col;
            uint8_t on = (rowData >> bitIndex) & 1;

            GPIO_WritePin(&ledMatrixCfg[cfg].colPins[col],
                          on ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }

        GPIO_WritePin(&ledMatrixCfg[cfg].rowPins[row], GPIO_PIN_RESET);

        for (volatile uint32_t d = 0; d < 100000; d++);
    }
}

#include "interface/HAL/keypad.h"

const uint8_t keypad_matrix[KEYPAD_ROWS][KEYPAD_COLOUMNS] = { {'1','2','3','A'}, {'4','5','6','B'}, {'7','8','9','C'}, {'*','0','#','D'} };

keypad_t Keypad[KEYPAD_LEN] = {
    [KEYPAD_0].rowPins = {
        {.port = GPIO_PORTA, .pin = GPIO_PIN_0, .mode = GPIO_MODE_OUTPUT, .outputType = GPIO_OUTPUT_PUSHPULL}, // Row 0
        {.port = GPIO_PORTA, .pin = GPIO_PIN_1, .mode = GPIO_MODE_OUTPUT, .outputType = GPIO_OUTPUT_PUSHPULL}, // Row 1
        {.port = GPIO_PORTA, .pin = GPIO_PIN_2, .mode = GPIO_MODE_OUTPUT, .outputType = GPIO_OUTPUT_PUSHPULL}, // Row 2
        {.port = GPIO_PORTA, .pin = GPIO_PIN_3, .mode = GPIO_MODE_OUTPUT, .outputType = GPIO_OUTPUT_PUSHPULL}  // Row 3
    },
    [KEYPAD_0].colPins = {
        {.port = GPIO_PORTA, .pin = GPIO_PIN_4, .mode = GPIO_MODE_INPUT, .pullType = GPIO_PULLDOWN}, // Column 0
        {.port = GPIO_PORTA, .pin = GPIO_PIN_5, .mode = GPIO_MODE_INPUT, .pullType = GPIO_PULLDOWN}, // Column 1
        {.port = GPIO_PORTA, .pin = GPIO_PIN_6, .mode = GPIO_MODE_INPUT, .pullType = GPIO_PULLDOWN}, // Column 2
        {.port = GPIO_PORTA, .pin = GPIO_PIN_7, .mode = GPIO_MODE_INPUT, .pullType = GPIO_PULLDOWN}  // Column 3
    }
};
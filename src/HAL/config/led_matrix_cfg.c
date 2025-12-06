#include "configuration/HAL/led_matrix_cfg.h"
#include "interface/HAL/led_matrix.h"

LEDMATRIX_Cfg_t ledMatrixCfg[LEDMATRIX_LEN] = {
    [LEDMATRIX_0] = {
        .rowPins = {
            { .port = GPIO_PORTA, .pin = GPIO_PIN_0, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTA, .pin = GPIO_PIN_1, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTA, .pin = GPIO_PIN_2, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTA, .pin = GPIO_PIN_3, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTA, .pin = GPIO_PIN_4, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTA, .pin = GPIO_PIN_5, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTA, .pin = GPIO_PIN_6, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTA, .pin = GPIO_PIN_7, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 }
        },
        .colPins = {
            { .port = GPIO_PORTB, .pin = GPIO_PIN_0, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTB, .pin = GPIO_PIN_1, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTB, .pin = GPIO_PIN_2, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTB, .pin = GPIO_PIN_3, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTB, .pin = GPIO_PIN_4, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTB, .pin = GPIO_PIN_5, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTB, .pin = GPIO_PIN_6, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 },
            { .port = GPIO_PORTB, .pin = GPIO_PIN_7, .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .outputType = GPIO_OUTPUT_PUSHPULL, .pullType = GPIO_OUTPUT_PUSHPULL, .altFunc = 0 }
        }
    }
};
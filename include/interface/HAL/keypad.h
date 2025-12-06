#ifndef KEYPAD_H
#define KEYPAD_H

#include "../../../lib/STD_Types.h"
#include "interface/MCAL/gpio.h"
#include "configuration/HAL/keypad_cfg.h"

#define KEYPAD_ROWS     4
#define KEYPAD_COLOUMNS 4

typedef struct{
	GPIO_t rowPins[KEYPAD_ROWS];
    GPIO_t colPins[KEYPAD_COLOUMNS];
}keypad_t;

STD_ReturnType keypad_Init();
STD_ReturnType keypad_GetKey(uint8_t *key);


#endif // KEYPAD_H
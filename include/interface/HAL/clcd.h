#ifndef CLCD_H
#define CLCD_H

#include "interface/MCAL/gpio.h"
#include "configuration/HAL/clcd_cfg.h"
#include "../../../lib/STD_Types.h"

#define LCD_CLEAR                                   0x01
#define LCD_CURSOR_HOME                             0x02

#define LCD_DECREMENT_SHIFT_OFF                     0x04
#define LCD_DECREMENT_SHIFT_ON                      0x05
#define LCD_INCREMENT_SHIFT_OFF                     0x06
#define LCD_INCREMENT_SHIFT_ON                      0x07

#define LCD_DISPLAY_OFF_UNDERLINE_OFF_CURSOR_OFF    0x08
#define LCD_DISPLAY_ON_UNDERLINE_OFF_CURSOR_OFF     0x0C
#define LCD_DISPLAY_ON_UNDERLINE_OFF_CURSOR_ON      0x0D
#define LCD_DISPLAY_ON_UNDERLINE_ON_CURSOR_OFF      0x0E
#define LCD_DISPLAY_ON_UNDERLINE_ON_CURSOR_ON       0x0F

#define LCD_MOVE_CURSOR_SHIFT_LEFT                  0x10
#define LCD_MOVE_CURSOR_SHIFT_RIGHT                 0x14
#define LCD_SHIFT_DISPLAY_SHIFT_LEFT                0x18
#define LCD_SHIFT_DISPLAY_SHIFT_RIGHT               0x1C

#define LCD_4BIT_MODE                               0x02
#define LCD_8BIT_MODE                               0x03
#define LCD_4BIT_MODE_2LINES                        0x28

#define LCD_CGRAM_START                             0x40
#define LCD_DDRAM_START                             0x80

#define NUMBER_OF_ROWS          2
#define NUMBER_OF_COLOMNS       16

#define ROW1    1
#define ROW2    2
 
typedef struct
{
    GPIO_t rsPin;
    GPIO_t rwPin;
    GPIO_t enPin;
    GPIO_t dataPins[4];
} CLCD_t;

STD_ReturnType CLCD_Init();
STD_ReturnType CLCD_WriteCommand(CLCD_Instance_t clcd, uint8_t command);
STD_ReturnType CLCD_WriteChar(CLCD_Instance_t clcd, uint8_t data);
STD_ReturnType CLCD_WriteCharPos(CLCD_Instance_t clcd, uint8_t row, uint8_t colomn, uint8_t data);
STD_ReturnType CLCD_WriteString(CLCD_Instance_t clcd, uint8_t *string);
STD_ReturnType CLCD_WriteStringPos(CLCD_Instance_t clcd, uint8_t row, uint8_t colomn, uint8_t *string);
STD_ReturnType CLCD_WriteCustomCharacter(CLCD_Instance_t clcd, uint8_t row, uint8_t colomn, const uint8_t ch[], uint8_t mem_pos);

#endif  /* CLCD_H */
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

typedef enum{
    CLCD_ASYNC_NOACTION = 0,
    CLCD_ASYNC_INIT,
    CLCD_ASYNC_WRITE_DATA,
    CLCD_ASYNC_WRITE_COMMAND,
    CLCD_ASYNC_WRITE_DATA_POS,
    CLCD_ASYNC_SAVE_CUSTOM_CHARACTER,
    CLCD_ASYNC_WRITE_CUSTOM_CHARACTER
} CLCD_ASYNC_State_t;

typedef enum{
    CLCD_ASYNC_NO_ACTION = 0,

    CLCD_ASYNC_FUNCTION_SET_1_HIGHNIBBLE_EN,
    CLCD_ASYNC_FUNCTION_SET_1_HIGHNIBBLE_DIS,
    CLCD_ASYNC_FUNCTION_SET_1_LOWNIBBLE_EN,
    CLCD_ASYNC_FUNCTION_SET_1_LOWNIBBLE_DIS,

    CLCD_ASYNC_FUNCTION_SET_2_HIGHNIBBLE_EN,
    CLCD_ASYNC_FUNCTION_SET_2_HIGHNIBBLE_DIS,
    CLCD_ASYNC_FUNCTION_SET_2_LOWNIBBLE_EN,
    CLCD_ASYNC_FUNCTION_SET_2_LOWNIBBLE_DIS,

    CLCD_ASYNC_FUNCTION_SET_3_HIGHNIBBLE_EN,
    CLCD_ASYNC_FUNCTION_SET_3_HIGHNIBBLE_DIS,
    CLCD_ASYNC_FUNCTION_SET_3_LOWNIBBLE_EN,
    CLCD_ASYNC_FUNCTION_SET_3_LOWNIBBLE_DIS,

    CLCD_ASYNC_FUNCTION_SET_4_HIGHNIBBLE_EN,
    CLCD_ASYNC_FUNCTION_SET_4_HIGHNIBBLE_DIS,
    CLCD_ASYNC_FUNCTION_SET_4_LOWNIBBLE_EN,
    CLCD_ASYNC_FUNCTION_SET_4_LOWNIBBLE_DIS,

    CLCD_ASYNC_FUNCTION_SET_5_HIGHNIBBLE_EN,
    CLCD_ASYNC_FUNCTION_SET_5_HIGHNIBBLE_DIS,
    CLCD_ASYNC_FUNCTION_SET_5_LOWNIBBLE_EN,
    CLCD_ASYNC_FUNCTION_SET_5_LOWNIBBLE_DIS,

    CLCD_ASYNC_DISPLAY_ON_HIGHNIBBLE_EN,
    CLCD_ASYNC_DISPLAY_ON_HIGHNIBBLE_DIS,
    CLCD_ASYNC_DISPLAY_ON_LOWNIBBLE_EN,
    CLCD_ASYNC_DISPLAY_ON_LOWNIBBLE_DIS,

    CLCD_ASYNC_CLEAR_HIGHNIBBLE_EN,
    CLCD_ASYNC_CLEAR_HIGHNIBBLE_DIS,
    CLCD_ASYNC_CLEAR_LOWNIBBLE_EN,
    CLCD_ASYNC_CLEAR_LOWNIBBLE_DIS,

    CLCD_ASYNC_ENTRY_MODE_HIGHNIBBLE_EN,
    CLCD_ASYNC_ENTRY_MODE_HIGHNIBBLE_DIS,
    CLCD_ASYNC_ENTRY_MODE_LOWNIBBLE_EN,
    CLCD_ASYNC_ENTRY_MODE_LOWNIBBLE_DIS,

    CLCD_ASYNC_INIT_DONE
} CLCD_ASYNC_INIT_State_t;

typedef enum{
    CLCD_ASYNC_WRITE_HIGHNIBBLE_EN = 0,
    CLCD_ASYNC_WRITE_HIGHNIBBLE_DIS,
    CLCD_ASYNC_WRITE_LOWNIBBLE_EN,
    CLCD_ASYNC_WRITE_LOWNIBBLE_DIS,
} CLCD_ASYNC_Write_t;
 
typedef struct
{
    GPIO_t rsPin;
    GPIO_t rwPin;
    GPIO_t enPin;
    GPIO_t dataPins[4];
} CLCD_t;

STD_ReturnType CLCD_asyncInit();
STD_ReturnType CLCD_asyncWriteString(CLCD_Instance_t lcdName, uint8_t* string);
STD_ReturnType CLCD_asyncWriteStringPos(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t* string);
STD_ReturnType CLCD_asyncWriteCommand(CLCD_Instance_t lcdName, uint8_t command);
STD_ReturnType CLCD_asyncSaveCustomCharacter(CLCD_Instance_t lcdName, uint8_t* ch, uint8_t mem_pos);
STD_ReturnType CLCD_asyncWriteCustomCharacter(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t mem_pos);

STD_ReturnType CLCD_syncInit();
STD_ReturnType CLCD_syncWriteCommand(CLCD_Instance_t clcd, uint8_t command);
STD_ReturnType CLCD_syncWriteChar(CLCD_Instance_t clcd, uint8_t data);
STD_ReturnType CLCD_syncWriteCharPos(CLCD_Instance_t clcd, uint8_t row, uint8_t colomn, uint8_t data);
STD_ReturnType CLCD_syncWriteString(CLCD_Instance_t clcd, uint8_t *string);
STD_ReturnType CLCD_syncWriteStringPos(CLCD_Instance_t clcd, uint8_t row, uint8_t colomn, uint8_t *string);
STD_ReturnType CLCD_syncSaveCustomCharacter(CLCD_Instance_t lcdName, const uint8_t ch[], uint8_t mem_pos);
STD_ReturnType CLCD_syncWriteCustomCharacter(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t mem_pos);

#endif  /* CLCD_H */
#include "interface/HAL/clcd.h"
#include "interface/Core/systick.h"

static void CLCD_Write4bit(CLCD_Instance_t lcdName, uint8_t data);
static void CLCD_EnableSignal(CLCD_Instance_t lcdName);
static void CLCD_SetCursor(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn);

extern CLCD_t clcd[CLCD_LEN];

void CLCD_Init(void){
    for(uint8_t i = 0; i < CLCD_LEN; i++){
        clcd[i].rsPin = (GPIO_t){
            .port       = clcd[i].rsPin.port,
            .pin        = clcd[i].rsPin.pin,
            .mode       = GPIO_MODE_OUTPUT,
            .outputType =  GPIO_OUTPUT_PUSHPULL,
            .speed      = GPIO_SPEED_MEDIUM,
            .pullType   = GPIO_NOPULL,
            .altFunc    = GPIO_AF0_SYSTEM
        };
        GPIO_Init(&clcd[i].rsPin);

        clcd[i].rwPin = (GPIO_t){
            .port       = clcd[i].rwPin.port,
            .pin        = clcd[i].rwPin.pin,
            .mode       = GPIO_MODE_OUTPUT,
            .outputType = GPIO_OUTPUT_PUSHPULL,
            .speed      = GPIO_SPEED_MEDIUM,
            .pullType   = GPIO_NOPULL,
            .altFunc    = GPIO_AF0_SYSTEM
        };
        GPIO_Init(&clcd[i].rwPin);

        clcd[i].enPin = (GPIO_t){
            .port       = clcd[i].enPin.port,
            .pin        = clcd[i].enPin.pin,
            .mode       = GPIO_MODE_OUTPUT,
            .outputType = GPIO_OUTPUT_PUSHPULL,
            .speed      = GPIO_SPEED_MEDIUM,
            .pullType   = GPIO_NOPULL,
            .altFunc    = GPIO_AF0_SYSTEM
        };
        GPIO_Init(&clcd[i].enPin);

        for(uint8_t j = 0; j < 4; j++){
            clcd[i].dataPins[j] = (GPIO_t){
                .port       = clcd[i].dataPins[j].port,
                .pin        = clcd[i].dataPins[j].pin,
                .mode       = GPIO_MODE_OUTPUT,
                .outputType = GPIO_OUTPUT_PUSHPULL,
                .speed      = GPIO_SPEED_MEDIUM,
                .pullType   = GPIO_NOPULL,
                .altFunc    = GPIO_AF0_SYSTEM
            };
            GPIO_Init(&clcd[i].dataPins[j]);
        }
    }

	for(CLCD_Instance_t i = 0; i < CLCD_LEN; i++){
		SYSTICK_DelayMS(40);
		CLCD_WriteCommand(i, LCD_8BIT_MODE_2LINES);
        SYSTICK_DelayMS(5);
        CLCD_WriteCommand(i, LCD_8BIT_MODE_2LINES);
        SYSTICK_DelayMS(1);
        CLCD_WriteCommand(i, LCD_8BIT_MODE_2LINES);
        CLCD_WriteCommand(i, LCD_CLEAR);
        SYSTICK_DelayMS(2);
        CLCD_WriteCommand(i, LCD_CURSOR_HOME);
        SYSTICK_DelayMS(2);
        CLCD_WriteCommand(i, LCD_INCREMENT_SHIFT_OFF);
        SYSTICK_DelayMS(2);
        CLCD_WriteCommand(i, LCD_DISPLAY_ON_UNDERLINE_OFF_CURSOR_OFF);
        SYSTICK_DelayMS(2);

        CLCD_WriteCommand(i, LCD_4BIT_MODE_2LINES);
        SYSTICK_DelayMS(2);
        CLCD_WriteCommand(i, LCD_DDRAM_START);
	}
}

void CLCD_WriteCommand(CLCD_Instance_t lcdName, uint8_t command){
	if(lcdName < CLCD_LEN){
        GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_RESET);    // Send Command
        CLCD_Write4bit(lcdName, command >> 4);                         // Send Higher Nibble
        CLCD_EnableSignal(lcdName);                                          // Enable Pulse           
        CLCD_Write4bit(lcdName, command);                              // Send Lower Nibble          
        CLCD_EnableSignal(lcdName);                                          // Enable Pulse             
	}
	else{ /* Do Nothing */ }
}

void CLCD_WriteChar(CLCD_Instance_t lcdName, uint8_t data){
	if(lcdName < CLCD_LEN){
        GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_SET);      // Send Data
        CLCD_Write4bit(lcdName, data >> 4);                                  // Send Higher Nibble                    
        CLCD_EnableSignal(lcdName);                                    // Enable Pulse   
        CLCD_Write4bit(lcdName, data);                                       // Send Lower Nibble          
        CLCD_EnableSignal(lcdName);                                    // Enable Pulse   
	}
	else{ /* Do Nothing */ }
}

void CLCD_WriteCharPos(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t data)
{
	if(lcdName < CLCD_LEN)
	{
		CLCD_SetCursor(lcdName, row, colomn);
		CLCD_WriteChar(lcdName, data);
	}
	else{ /* Do Nothing */ }
}

void CLCD_WriteString(CLCD_Instance_t lcdName, uint8_t *string)
{
	if(lcdName < CLCD_LEN && NULL != string)
	{
        do{
        	CLCD_WriteChar(lcdName, *string);
        } while(*(++string));
	}
	else{ /* Do Nothing */ }
}

void CLCD_WriteStringPos(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t *string)
{
	if(lcdName < CLCD_LEN && NULL != string)
	{
		CLCD_SetCursor(lcdName, row, colomn);
		CLCD_WriteString(lcdName, string);
	}
	else{ /* Do Nothing */ }
}

void CLCD_WriteCustomCharacter(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn,
                                                const uint8_t ch[], uint8_t mem_pos)
{
	if(lcdName < CLCD_LEN)
	{
		CLCD_WriteCommand(lcdName, (LCD_CGRAM_START + (mem_pos * 8)));
        for(uint8_t char_index = 0 ; char_index < 8 ; char_index++){
        	CLCD_WriteChar(lcdName, ch[char_index]);
        }
        CLCD_WriteCharPos(lcdName, row, colomn, mem_pos);
	}
	else{ /* Do Nothing */ }
}

static void CLCD_Write4bit(CLCD_Instance_t lcdName, uint8_t data){
    if(lcdName < CLCD_LEN){
        for(uint8_t line_pins = 0 ; line_pins < 4 ; line_pins++){
            GPIO_WritePin(&clcd[lcdName].dataPins[line_pins], (data >> line_pins) & 0x01);
        }
    }
    else{ /* Do Nothing */ }
}

static void CLCD_EnableSignal(CLCD_Instance_t lcdName){
    if(lcdName < CLCD_LEN){
        GPIO_WritePin(&clcd[lcdName].enPin, GPIO_PIN_SET);
        SYSTICK_DelayMS(3);
        GPIO_WritePin(&clcd[lcdName].enPin, GPIO_PIN_RESET);
    }
    else{ /* Do Nothing */ }
}

static void CLCD_SetCursor(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn){
    if(lcdName < CLCD_LEN && row <= NUMBER_OF_ROWS && colomn <= NUMBER_OF_COLOMNS)
    {
        colomn--;
        switch(row)
        {
            case ROW1: CLCD_WriteCommand(lcdName, (0x80 + colomn));  break;
            case ROW2: CLCD_WriteCommand(lcdName, (0xc0 + colomn));  break;
            default:                                            break;
        }
    }
}

#include "interface/HAL/clcd.h"
#include "interface/Core/systick.h"

static STD_ReturnType CLCD_Write4bit(CLCD_Instance_t lcdName, uint8_t data);
static STD_ReturnType CLCD_EnableSignal(CLCD_Instance_t lcdName);
static STD_ReturnType CLCD_SetCursor(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn);

extern CLCD_t clcd[CLCD_LEN];

STD_ReturnType CLCD_Init(void){
    STD_ReturnType ret = STD_SUCCESS;

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
        ret = GPIO_Init(&clcd[i].rsPin);

        clcd[i].rwPin = (GPIO_t){
            .port       = clcd[i].rwPin.port,
            .pin        = clcd[i].rwPin.pin,
            .mode       = GPIO_MODE_OUTPUT,
            .outputType = GPIO_OUTPUT_PUSHPULL,
            .speed      = GPIO_SPEED_MEDIUM,
            .pullType   = GPIO_NOPULL,
            .altFunc    = GPIO_AF0_SYSTEM
        };
        ret = GPIO_Init(&clcd[i].rwPin);

        clcd[i].enPin = (GPIO_t){
            .port       = clcd[i].enPin.port,
            .pin        = clcd[i].enPin.pin,
            .mode       = GPIO_MODE_OUTPUT,
            .outputType = GPIO_OUTPUT_PUSHPULL,
            .speed      = GPIO_SPEED_MEDIUM,
            .pullType   = GPIO_NOPULL,
            .altFunc    = GPIO_AF0_SYSTEM
        };
        ret = GPIO_Init(&clcd[i].enPin);

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
            ret = GPIO_Init(&clcd[i].dataPins[j]);
        }

        if(ret != STD_SUCCESS){
            break;
        }
    }

	for(CLCD_Instance_t i = 0; i < CLCD_LEN; i++){
        if(ret != STD_SUCCESS){
            break;
        }
		ret = SYSTICK_DelayMS(40);
		ret = CLCD_WriteCommand(i, LCD_8BIT_MODE_2LINES);
        ret = SYSTICK_DelayMS(5);
        ret = CLCD_WriteCommand(i, LCD_8BIT_MODE_2LINES);
        ret = SYSTICK_DelayMS(1);
        ret = CLCD_WriteCommand(i, LCD_8BIT_MODE_2LINES);
        ret = CLCD_WriteCommand(i, LCD_CLEAR);
        ret = SYSTICK_DelayMS(2);
        ret = CLCD_WriteCommand(i, LCD_CURSOR_HOME);
        ret = SYSTICK_DelayMS(2);
        ret = CLCD_WriteCommand(i, LCD_INCREMENT_SHIFT_OFF);
        ret = SYSTICK_DelayMS(2);
        ret = CLCD_WriteCommand(i, LCD_DISPLAY_ON_UNDERLINE_OFF_CURSOR_OFF);
        ret = SYSTICK_DelayMS(2);

        ret = CLCD_WriteCommand(i, LCD_4BIT_MODE_2LINES);
        ret = SYSTICK_DelayMS(2);
        ret = CLCD_WriteCommand(i, LCD_DDRAM_START);
	}

    return ret;
}

STD_ReturnType CLCD_WriteCommand(CLCD_Instance_t lcdName, uint8_t command){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN){
        ret = GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_RESET);    // Send Command
        ret = CLCD_Write4bit(lcdName, command >> 4);                         // Send Higher Nibble
        ret = CLCD_EnableSignal(lcdName);                                          // Enable Pulse           
        ret = CLCD_Write4bit(lcdName, command);                              // Send Lower Nibble          
        ret = CLCD_EnableSignal(lcdName);                                          // Enable Pulse             
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_WriteChar(CLCD_Instance_t lcdName, uint8_t data){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN){
        ret = GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_SET);      // Send Data
        ret = CLCD_Write4bit(lcdName, data >> 4);                                  // Send Higher Nibble                    
        ret = CLCD_EnableSignal(lcdName);                                    // Enable Pulse   
        ret = CLCD_Write4bit(lcdName, data);                                       // Send Lower Nibble          
        ret = CLCD_EnableSignal(lcdName);                                    // Enable Pulse   
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_WriteCharPos(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t data){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN){
		ret = CLCD_SetCursor(lcdName, row, colomn);
		ret = CLCD_WriteChar(lcdName, data);
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_WriteString(CLCD_Instance_t lcdName, uint8_t *string){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN && NULL != string){
        do{
        	ret = CLCD_WriteChar(lcdName, *string);
        } while(*(++string));
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_WriteStringPos(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t *string){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN && NULL != string){
		ret = CLCD_SetCursor(lcdName, row, colomn);
		ret = CLCD_WriteString(lcdName, string);
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_WriteCustomCharacter(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn,
                                                const uint8_t ch[], uint8_t mem_pos){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN){
		ret = CLCD_WriteCommand(lcdName, (LCD_CGRAM_START + (mem_pos * 8)));
        for(uint8_t char_index = 0 ; char_index < 8 ; char_index++){
        	ret = CLCD_WriteChar(lcdName, ch[char_index]);
        }
        ret = CLCD_WriteCharPos(lcdName, row, colomn, mem_pos);
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

static STD_ReturnType CLCD_Write4bit(CLCD_Instance_t lcdName, uint8_t data){
    STD_ReturnType ret = STD_SUCCESS;

    if(lcdName < CLCD_LEN){
        for(uint8_t line_pins = 0 ; line_pins < 4 ; line_pins++){
            ret = GPIO_WritePin(&clcd[lcdName].dataPins[line_pins], (data >> line_pins) & 0x01);
        }
    }
    else{ 
        ret = STD_ERROR;
    }

    return ret;
}

static STD_ReturnType CLCD_EnableSignal(CLCD_Instance_t lcdName){
    STD_ReturnType ret = STD_SUCCESS;

    if(lcdName < CLCD_LEN){
        ret = GPIO_WritePin(&clcd[lcdName].enPin, GPIO_PIN_SET);
        ret = SYSTICK_DelayMS(3);
        ret = GPIO_WritePin(&clcd[lcdName].enPin, GPIO_PIN_RESET);
    }
    else{  
        ret = STD_ERROR;
    }

    return ret;
}

static STD_ReturnType CLCD_SetCursor(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn){
    STD_ReturnType ret = STD_SUCCESS;

    if(lcdName < CLCD_LEN && row <= NUMBER_OF_ROWS && colomn <= NUMBER_OF_COLOMNS){
        colomn--;
        switch(row)
        {
            case ROW1: ret = CLCD_WriteCommand(lcdName, (0x80 + colomn));  break;
            case ROW2: ret = CLCD_WriteCommand(lcdName, (0xc0 + colomn));  break;
            default:                                                                break;
        }
    }
    else{
        ret = STD_ERROR;
    }

    return ret;
}

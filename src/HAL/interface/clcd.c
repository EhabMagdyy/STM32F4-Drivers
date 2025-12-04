#include "interface/HAL/clcd.h"
#include "interface/Core/systick.h"
#include "OS/scheduler.h"

static STD_ReturnType CLCD_syncWrite4bit(CLCD_Instance_t lcdName, uint8_t data);
static STD_ReturnType CLCD_syncEnableSignal(CLCD_Instance_t lcdName);
static STD_ReturnType CLCD_syncSetCursor(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn);

static void CLCD_Runnable(void* arg);

extern CLCD_t clcd[CLCD_LEN];

/***
**
**
**********************************  ASYNCHRONOUS FUNCTIONS
**
**
***/

volatile CLCD_ASYNC_State_t clcd_async_state[CLCD_LEN] = {CLCD_ASYNC_NOACTION};
volatile CLCD_ASYNC_INIT_State_t clcd_async_init_state[CLCD_LEN] = {CLCD_ASYNC_NO_ACTION};
volatile CLCD_ASYNC_Write_t clcd_async_write_data_state[CLCD_LEN] = {CLCD_ASYNC_WRITE_HIGHNIBBLE_EN};
volatile CLCD_ASYNC_Write_t clcd_async_write_command_state[CLCD_LEN] = {CLCD_ASYNC_WRITE_HIGHNIBBLE_EN};
volatile uint8_t* requestedString[CLCD_LEN] = {NULL};
volatile uint8_t requestedCommand[CLCD_LEN] = {0};

// Custom Character
volatile uint8_t requestedCustomCharacter[8] = {0};

static Runnable__t CLCD_RunnableObj = {
    .callback = CLCD_Runnable,
    .Periodicity = 1,   // 1*3 = 3 ms
    .FirstDelay = 12,   // 12*3 = 36 ms
    .arg = 0
};

STD_ReturnType CLCD_asyncInit(void){
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
        else{
            ret = Scheduler_RegisterRunnable(&CLCD_RunnableObj);

            if(ret == STD_SUCCESS){
                clcd_async_state[i] = CLCD_ASYNC_INIT;
                clcd_async_init_state[i] = CLCD_ASYNC_FUNCTION_SET_1_HIGHNIBBLE_EN;
            }
        }
    }

    return ret;
}

static STD_ReturnType CLCD_asyncEnableSignal(CLCD_Instance_t lcdName){
    STD_ReturnType ret = STD_SUCCESS;

    if(lcdName < CLCD_LEN){
        ret = GPIO_WritePin(&clcd[lcdName].enPin, GPIO_PIN_SET);
    }
    else{  
        ret = STD_ERROR;
    }

    return ret;
}

static STD_ReturnType CLCD_asyncDisableSignal(CLCD_Instance_t lcdName){
    STD_ReturnType ret = STD_SUCCESS;

    if(lcdName < CLCD_LEN){
        ret = GPIO_WritePin(&clcd[lcdName].enPin, GPIO_PIN_RESET);
    }
    else{  
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_asyncWriteString(CLCD_Instance_t lcdName, uint8_t* string){
    STD_ReturnType ret = STD_SUCCESS;

    if(lcdName < CLCD_LEN){
        if(clcd_async_init_state[lcdName] == CLCD_ASYNC_INIT_DONE && clcd_async_state[lcdName] == CLCD_ASYNC_NOACTION){
            if(string != NULL){
                clcd_async_state[lcdName] = CLCD_ASYNC_WRITE_DATA;
                requestedString[lcdName] = string;
            }
            else{
                ret = STD_ERROR;
            }
        }
        else{ 
            ret = STD_ERROR;
        }
    }
    else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_asyncWriteStringPos(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t* string){
    STD_ReturnType ret = STD_SUCCESS;

    if(lcdName < CLCD_LEN){
        if(clcd_async_init_state[lcdName] == CLCD_ASYNC_INIT_DONE && clcd_async_state[lcdName] == CLCD_ASYNC_NOACTION){
            if(string != NULL){
                if(lcdName < CLCD_LEN && row <= NUMBER_OF_ROWS && colomn <= NUMBER_OF_COLOMNS){
                    colomn--;
                    switch(row){
                        case ROW1: requestedCommand[lcdName] = 0x80 + colomn;  break;
                        case ROW2: requestedCommand[lcdName] = 0xc0 + colomn;  break;
                        default:                                      break;
                    }
                }
                else{
                    ret = STD_ERROR;
                }
                clcd_async_state[lcdName] = CLCD_ASYNC_WRITE_DATA_POS;
                requestedString[lcdName] = string;
            }
            else{
                ret = STD_ERROR;
            }
        }
        else{ 
            ret = STD_ERROR;
        }
    }
    else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_asyncWriteCommand(CLCD_Instance_t lcdName, uint8_t command){
    STD_ReturnType ret = STD_SUCCESS;

    if(lcdName < CLCD_LEN){
        if(clcd_async_init_state[lcdName] == CLCD_ASYNC_INIT_DONE && clcd_async_state[lcdName] == CLCD_ASYNC_NOACTION){
            clcd_async_state[lcdName] = CLCD_ASYNC_WRITE_COMMAND;
            requestedCommand[lcdName] = command;
        }
        else{ 
            ret = STD_ERROR;
        }
    }
    else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_asyncSaveCustomCharacter(CLCD_Instance_t lcdName, uint8_t* ch, uint8_t mem_pos){
    STD_ReturnType ret = STD_SUCCESS;

    if(lcdName < CLCD_LEN){
        if(clcd_async_init_state[lcdName] == CLCD_ASYNC_INIT_DONE && clcd_async_state[lcdName] == CLCD_ASYNC_NOACTION){
            if(ch != NULL){
                for(uint8_t i = 0; i < 8; i++){
                    requestedCustomCharacter[i] = ch[i];
                }
                requestedCommand[lcdName] = LCD_CGRAM_START + (mem_pos * 8);
                clcd_async_state[lcdName] = CLCD_ASYNC_SAVE_CUSTOM_CHARACTER;
            }
            else{
                ret = STD_ERROR;
            }
        }
        else{
            ret = STD_ERROR;
        }
    }
    else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_asyncWriteCustomCharacter(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t mem_pos){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN){
        ret = CLCD_asyncWriteStringPos(lcdName, row, colomn, (uint8_t*)&mem_pos);
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

static void CLCD_asyncWrite4bitEn(CLCD_Instance_t lcdName, uint8_t data){
    CLCD_syncWrite4bit(lcdName, data);
    CLCD_asyncEnableSignal(lcdName);
}

static void CLCD_Runnable(void* arg){
    uint8_t lcdName = *(uint8_t*)arg;
    switch(clcd_async_state[lcdName]){
        case CLCD_ASYNC_NOACTION:
            // Do nothing
            break;

        case CLCD_ASYNC_INIT:
            switch(clcd_async_init_state[lcdName]++){
                // Function Set - 8 bit mode
                case CLCD_ASYNC_FUNCTION_SET_1_HIGHNIBBLE_EN:
                    GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_RESET);
                    CLCD_asyncWrite4bitEn(lcdName, LCD_8BIT_MODE >> 4);
                    break;
                case CLCD_ASYNC_FUNCTION_SET_1_HIGHNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                case CLCD_ASYNC_FUNCTION_SET_1_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_8BIT_MODE & 0x0F);
                    break;
                case CLCD_ASYNC_FUNCTION_SET_1_LOWNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                // Function Set - 8 bit mode
                case CLCD_ASYNC_FUNCTION_SET_2_HIGHNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_8BIT_MODE >> 4);
                    break;
                case CLCD_ASYNC_FUNCTION_SET_2_HIGHNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                case CLCD_ASYNC_FUNCTION_SET_2_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_8BIT_MODE & 0x0F);
                    break;
                case CLCD_ASYNC_FUNCTION_SET_2_LOWNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                // Function Set - 8 bit mode
                case CLCD_ASYNC_FUNCTION_SET_3_HIGHNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_8BIT_MODE >> 4);
                    break;
                case CLCD_ASYNC_FUNCTION_SET_3_HIGHNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                case CLCD_ASYNC_FUNCTION_SET_3_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_8BIT_MODE & 0x0F);
                    break;
                    break;
                case CLCD_ASYNC_FUNCTION_SET_3_LOWNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break; 
                // Function Set - 4 bit mode
                case CLCD_ASYNC_FUNCTION_SET_4_HIGHNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_4BIT_MODE >> 4);
                    break;
                case CLCD_ASYNC_FUNCTION_SET_4_HIGHNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                case CLCD_ASYNC_FUNCTION_SET_4_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_4BIT_MODE & 0x0F);
                    break;
                    break;
                case CLCD_ASYNC_FUNCTION_SET_4_LOWNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                    break;
                // Function Set - 4 bit mode - Two Lines
                case CLCD_ASYNC_FUNCTION_SET_5_HIGHNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_4BIT_MODE_2LINES >> 4);
                    break;
                case CLCD_ASYNC_FUNCTION_SET_5_HIGHNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                case CLCD_ASYNC_FUNCTION_SET_5_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_4BIT_MODE_2LINES & 0x0F);
                    break;
                    break;
                case CLCD_ASYNC_FUNCTION_SET_5_LOWNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                // Display ON
                case CLCD_ASYNC_DISPLAY_ON_HIGHNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_DISPLAY_ON_UNDERLINE_OFF_CURSOR_OFF >> 4);
                    break;
                case CLCD_ASYNC_DISPLAY_ON_HIGHNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                case CLCD_ASYNC_DISPLAY_ON_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_DISPLAY_ON_UNDERLINE_OFF_CURSOR_OFF & 0x0F);
                    break;
                case CLCD_ASYNC_DISPLAY_ON_LOWNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                // Clear Display
                case CLCD_ASYNC_CLEAR_HIGHNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_CLEAR >> 4);
                    break;
                case CLCD_ASYNC_CLEAR_HIGHNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                case CLCD_ASYNC_CLEAR_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_CLEAR & 0x0F);
                    break;
                case CLCD_ASYNC_CLEAR_LOWNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                // Entry Mode
                case CLCD_ASYNC_ENTRY_MODE_HIGHNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_INCREMENT_SHIFT_OFF >> 4);
                    break;
                case CLCD_ASYNC_ENTRY_MODE_HIGHNIBBLE_DIS: CLCD_asyncDisableSignal(lcdName); break;
                case CLCD_ASYNC_ENTRY_MODE_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, LCD_INCREMENT_SHIFT_OFF & 0x0F);
                    break;
                case CLCD_ASYNC_ENTRY_MODE_LOWNIBBLE_DIS:
                    CLCD_asyncDisableSignal(lcdName);
                    clcd_async_state[lcdName] = CLCD_ASYNC_NO_ACTION;
                    break;

                default:
                    clcd_async_state[lcdName] = CLCD_ASYNC_NOACTION;
                    break;
            }
            break;

        case CLCD_ASYNC_WRITE_DATA:
            if(requestedString[lcdName] != NULL){
                switch(clcd_async_write_data_state[lcdName]++){
                    case CLCD_ASYNC_WRITE_HIGHNIBBLE_EN:
                        GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_SET);       // Send Data
                        CLCD_asyncWrite4bitEn(lcdName, (*requestedString[lcdName]) >> 4);
                        break;
                    case CLCD_ASYNC_WRITE_HIGHNIBBLE_DIS:
                        CLCD_asyncDisableSignal(lcdName);
                        break;
                    case CLCD_ASYNC_WRITE_LOWNIBBLE_EN:
                        CLCD_asyncWrite4bitEn(lcdName, (*requestedString[lcdName]) & 0x0F);
                        break;
                    case CLCD_ASYNC_WRITE_LOWNIBBLE_DIS:
                        CLCD_asyncDisableSignal(lcdName);
                        requestedString[lcdName]++;
                        if(*requestedString[lcdName] == '\0'){
                            clcd_async_state[lcdName] = CLCD_ASYNC_NOACTION;
                        }
                        clcd_async_write_data_state[lcdName] = CLCD_ASYNC_WRITE_HIGHNIBBLE_EN;
                        break;

                    default:
                        clcd_async_write_data_state[lcdName] = CLCD_ASYNC_WRITE_HIGHNIBBLE_EN;
                        break;
                }
            }
            else{
                clcd_async_state[lcdName] = CLCD_ASYNC_NOACTION;
            }
            break;

        case CLCD_ASYNC_WRITE_COMMAND:
            switch(clcd_async_write_command_state[lcdName]++){
                case CLCD_ASYNC_WRITE_HIGHNIBBLE_EN:
                    GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_RESET);       // Send Command
                    CLCD_asyncWrite4bitEn(lcdName, (*requestedString[lcdName]) >> 4);
                    break;
                case CLCD_ASYNC_WRITE_HIGHNIBBLE_DIS:
                    CLCD_asyncDisableSignal(lcdName);
                    break;
                case CLCD_ASYNC_WRITE_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, requestedCommand[lcdName] & 0x0F);
                    break;
                case CLCD_ASYNC_WRITE_LOWNIBBLE_DIS:
                    CLCD_asyncDisableSignal(lcdName);
                    clcd_async_write_command_state[lcdName] = CLCD_ASYNC_WRITE_HIGHNIBBLE_EN;
                    clcd_async_state[lcdName] = CLCD_ASYNC_NOACTION;
                    break;

                default:
                    clcd_async_write_command_state[lcdName] = CLCD_ASYNC_WRITE_HIGHNIBBLE_EN;
                    break;
                }
            break;

        case CLCD_ASYNC_WRITE_DATA_POS:
            switch(clcd_async_write_command_state[lcdName]++){
                case CLCD_ASYNC_WRITE_HIGHNIBBLE_EN:
                    GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_RESET);       // Send Command
                    CLCD_asyncWrite4bitEn(lcdName, requestedCommand[lcdName] >> 4);
                    break;
                case CLCD_ASYNC_WRITE_HIGHNIBBLE_DIS:
                    CLCD_asyncDisableSignal(lcdName);
                    break;
                case CLCD_ASYNC_WRITE_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, requestedCommand[lcdName] & 0x0F);
                    break;
                case CLCD_ASYNC_WRITE_LOWNIBBLE_DIS:
                    CLCD_asyncDisableSignal(lcdName);
                    clcd_async_write_command_state[lcdName] = CLCD_ASYNC_WRITE_HIGHNIBBLE_EN;
                    clcd_async_state[lcdName] = CLCD_ASYNC_WRITE_DATA;
                    break;

                default:
                    clcd_async_write_command_state[lcdName] = CLCD_ASYNC_WRITE_HIGHNIBBLE_EN;
                    break;
                }
            break;

        case CLCD_ASYNC_SAVE_CUSTOM_CHARACTER:
            switch(clcd_async_write_command_state[lcdName]++){
                case CLCD_ASYNC_WRITE_HIGHNIBBLE_EN:
                    GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_RESET);       // Send Command
                    CLCD_asyncWrite4bitEn(lcdName, requestedCommand[lcdName] >> 4);
                    break;
                case CLCD_ASYNC_WRITE_HIGHNIBBLE_DIS:
                    CLCD_asyncDisableSignal(lcdName);
                    break;
                case CLCD_ASYNC_WRITE_LOWNIBBLE_EN:
                    CLCD_asyncWrite4bitEn(lcdName, requestedCommand[lcdName] & 0x0F);
                    break;
                case CLCD_ASYNC_WRITE_LOWNIBBLE_DIS:
                    CLCD_asyncDisableSignal(lcdName);
                    clcd_async_write_command_state[lcdName] = CLCD_ASYNC_WRITE_HIGHNIBBLE_EN;
                    clcd_async_state[lcdName] = CLCD_ASYNC_WRITE_DATA;
                    requestedString[lcdName] = (uint8_t*)requestedCustomCharacter;
                    break;

                default:
                    clcd_async_write_command_state[lcdName] = CLCD_ASYNC_WRITE_HIGHNIBBLE_EN;
                    break;
                }
            break;
            
        default:
            // Invalid state, reset to NOACTION
            clcd_async_state[lcdName] = CLCD_ASYNC_NOACTION;
            break;
    }
}

/***
**
**
**********************************  SYNCHRONOUS FUNCTIONS
**
**
***/

STD_ReturnType CLCD_syncInit(void){
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
        ret = CLCD_syncWriteCommand(i, LCD_8BIT_MODE);
        ret = SYSTICK_DelayMS(5);
        ret = CLCD_syncWriteCommand(i, LCD_8BIT_MODE);
        ret = SYSTICK_DelayMS(1);
        ret = CLCD_syncWriteCommand(i, LCD_8BIT_MODE);
        ret = SYSTICK_DelayMS(2);
        ret = CLCD_syncWriteCommand(i, LCD_4BIT_MODE);
        ret = SYSTICK_DelayMS(2);
        ret = CLCD_syncWriteCommand(i, LCD_4BIT_MODE_2LINES);
        ret = SYSTICK_DelayMS(2);
        ret = CLCD_syncWriteCommand(i, LCD_DISPLAY_ON_UNDERLINE_OFF_CURSOR_OFF);
        ret = SYSTICK_DelayMS(2);
        ret = CLCD_syncWriteCommand(i, LCD_CLEAR);
        ret = SYSTICK_DelayMS(2);
        ret = CLCD_syncWriteCommand(i, LCD_INCREMENT_SHIFT_OFF);
        ret = SYSTICK_DelayMS(2);
	}

    return ret;
}

STD_ReturnType CLCD_syncWriteCommand(CLCD_Instance_t lcdName, uint8_t command){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN){
        ret = GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_RESET);    // Send Command
        ret = CLCD_syncWrite4bit(lcdName, command >> 4);                     // Send Higher Nibble
        ret = CLCD_syncEnableSignal(lcdName);                                      // Enable Pulse           
        ret = CLCD_syncWrite4bit(lcdName, command);                          // Send Lower Nibble          
        ret = CLCD_syncEnableSignal(lcdName);                                      // Enable Pulse             
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_syncWriteChar(CLCD_Instance_t lcdName, uint8_t data){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN){
        ret = GPIO_WritePin(&clcd[lcdName].rsPin, GPIO_PIN_SET);      // Send Data
        ret = CLCD_syncWrite4bit(lcdName, data >> 4);                                  // Send Higher Nibble                    
        ret = CLCD_syncEnableSignal(lcdName);                                    // Enable Pulse   
        ret = CLCD_syncWrite4bit(lcdName, data);                                       // Send Lower Nibble          
        ret = CLCD_syncEnableSignal(lcdName);                                    // Enable Pulse   
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_syncWriteCharPos(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t data){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN){
		ret = CLCD_syncSetCursor(lcdName, row, colomn);
		ret = CLCD_syncWriteChar(lcdName, data);
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_syncWriteString(CLCD_Instance_t lcdName, uint8_t *string){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN && NULL != string){
        do{
        	ret = CLCD_syncWriteChar(lcdName, *string);
        } while(*(++string));
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_syncWriteStringPos(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t *string){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN && NULL != string){
		ret = CLCD_syncSetCursor(lcdName, row, colomn);
		ret = CLCD_syncWriteString(lcdName, string);
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_syncSaveCustomCharacter(CLCD_Instance_t lcdName, const uint8_t ch[], uint8_t mem_pos){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN){
		ret = CLCD_syncWriteCommand(lcdName, (LCD_CGRAM_START + (mem_pos * 8)));
        for(uint8_t char_index = 0 ; char_index < 8 ; char_index++){
        	ret = CLCD_syncWriteChar(lcdName, ch[char_index]);
        }
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

STD_ReturnType CLCD_syncWriteCustomCharacter(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn, uint8_t mem_pos){
    STD_ReturnType ret = STD_SUCCESS;

	if(lcdName < CLCD_LEN){
        ret = CLCD_syncWriteCharPos(lcdName, row, colomn, mem_pos);
	}
	else{ 
        ret = STD_ERROR;
    }

    return ret;
}

static STD_ReturnType CLCD_syncWrite4bit(CLCD_Instance_t lcdName, uint8_t data){
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

static STD_ReturnType CLCD_syncEnableSignal(CLCD_Instance_t lcdName){
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

static STD_ReturnType CLCD_syncSetCursor(CLCD_Instance_t lcdName, uint8_t row, uint8_t colomn){
    STD_ReturnType ret = STD_SUCCESS;

    if(lcdName < CLCD_LEN && row <= NUMBER_OF_ROWS && colomn <= NUMBER_OF_COLOMNS){
        colomn--;
        switch(row)
        {
            case ROW1: ret = CLCD_syncWriteCommand(lcdName, (0x80 + colomn));  break;
            case ROW2: ret = CLCD_syncWriteCommand(lcdName, (0xc0 + colomn));  break;
            default:                                                                break;
        }
    }
    else{
        ret = STD_ERROR;
    }

    return ret;
}

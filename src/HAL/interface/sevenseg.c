
#include "interface/HAL/sevenseg.h"
#include "interface/MCAL/gpio.h"
#include "../../../lib/STD_Types.h" 

extern const SEVEN_SEGMENT_Config_t SEVEN_SEGMENTS[SEVEN_SEGMENT_LEN];

const uint8_t segmentsPattern[SEVEN_SEGMENT_MAX_NUMBER+1] = {
    0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
    0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111 
};

GPIO_t gpioSevenSegment[SEVEN_SEGMENT_LEN][SEVEN_SEGMENT_NUMBER_OF_SEGMENTS+1];

STD_ReturnType SevenSegment_Init(void){
    STD_ReturnType ret = STD_SUCCESS;

    for(volatile uint8_t NumOfSevSegs = 0; NumOfSevSegs < SEVEN_SEGMENT_LEN; NumOfSevSegs++){
        for(volatile uint8_t NumOfLeds = 0; NumOfLeds < SEVEN_SEGMENT_NUMBER_OF_SEGMENTS+2; NumOfLeds++){

            uint8_t pin = SEVEN_SEGMENTS[NumOfSevSegs].pins[NumOfLeds];

            gpioSevenSegment[NumOfSevSegs][NumOfLeds].port = SEVEN_SEGMENTS[NumOfSevSegs].port;
            gpioSevenSegment[NumOfSevSegs][NumOfLeds].pin = pin; 
            gpioSevenSegment[NumOfSevSegs][NumOfLeds].mode = GPIO_MODE_OUTPUT;
            gpioSevenSegment[NumOfSevSegs][NumOfLeds].speed = GPIO_SPEED_MEDIUM;
            gpioSevenSegment[NumOfSevSegs][NumOfLeds].outputType = GPIO_OUTPUT_PUSHPULL;
            gpioSevenSegment[NumOfSevSegs][NumOfLeds].pullType = GPIO_NOPULL;
            gpioSevenSegment[NumOfSevSegs][NumOfLeds].altFunc = GPIO_AF0_SYSTEM;

            ret = GPIO_Init(&gpioSevenSegment[NumOfSevSegs][NumOfLeds]);
            if(ret != STD_SUCCESS){
                ret = STD_ERROR;
                break;
            }
        }
        if(ret == STD_ERROR){
            break;
        }
        else{
            ret = SevenSegment_Enable(NumOfSevSegs);
        }
    }

    return ret;
}

STD_ReturnType SevenSegment_DeInit(void){
    STD_ReturnType ret = STD_SUCCESS;

    for(uint8_t i = 0; i < SEVEN_SEGMENT_LEN; i++){
        for(uint8_t j = 0; j < SEVEN_SEGMENT_NUMBER_OF_SEGMENTS+1; j++){
            ret = GPIO_DeInit(&gpioSevenSegment[i][j]);
            if(ret != STD_SUCCESS){
                break;
            }
        }
        if(ret == STD_ERROR){
            break;
        }
    }

    return ret;
}

STD_ReturnType SevenSegment_Write(SEVEN_SEGMENT_Names_t sevSegName, uint8_t number){
    STD_ReturnType ret = STD_SUCCESS;

    if(sevSegName >= SEVEN_SEGMENT_LEN || number > SEVEN_SEGMENT_MAX_NUMBER){
        ret = STD_ERROR;
    }
    else{
        uint8_t pattern = segmentsPattern[number];
        const uint8_t isCommonAnode = (SEVEN_SEGMENTS[sevSegName].type == SEVEN_SEGMENT_COMMON_ANODE);

        for(volatile uint8_t pin = 0; pin < SEVEN_SEGMENT_NUMBER_OF_SEGMENTS; pin++){
            GPIO_PinState_t pinState = ((pattern & (1 << pin)) != 0);
                        
            ret = GPIO_WritePin(&gpioSevenSegment[sevSegName][pin], pinState ^ isCommonAnode);
            if(ret != STD_SUCCESS){
                ret = STD_ERROR;
                break;
            }
        }
    }

    return ret;
}

STD_ReturnType SevenSegment_SetDotState(SEVEN_SEGMENT_Names_t sevSegName, SEVEN_SEGMENT_Dot_State_t state){
    STD_ReturnType ret = STD_SUCCESS;

    if(sevSegName >= SEVEN_SEGMENT_LEN){
        ret = STD_ERROR;
    }
    else{
        const uint8_t isCommonAnode = (SEVEN_SEGMENTS[sevSegName].type == SEVEN_SEGMENT_COMMON_ANODE);
             
        ret = GPIO_WritePin(&gpioSevenSegment[sevSegName][SEVEN_SEGMENTS[sevSegName].pins[7]], state ^ isCommonAnode);
        if(ret != STD_SUCCESS){
            ret = STD_ERROR;
        }
    }

    return ret;
}

STD_ReturnType SevenSegment_Enable(SEVEN_SEGMENT_Names_t sevSegName){
    STD_ReturnType ret = STD_SUCCESS;

    if(sevSegName >= SEVEN_SEGMENT_LEN){
        ret = STD_ERROR;
    }
    else{
        const uint8_t isCommonAnode = (SEVEN_SEGMENTS[sevSegName].type == SEVEN_SEGMENT_COMMON_ANODE);
             
        ret = GPIO_WritePin(&gpioSevenSegment[sevSegName][SEVEN_SEGMENTS[sevSegName].pins[8]], 0 ^ isCommonAnode);
        if(ret != STD_SUCCESS){
            ret = STD_ERROR;
        }
    }

    return ret;
}

STD_ReturnType SevenSegment_Disable(SEVEN_SEGMENT_Names_t sevSegName){
    STD_ReturnType ret = STD_SUCCESS;

    if(sevSegName >= SEVEN_SEGMENT_LEN){
        ret = STD_ERROR;
    }
    else{
        const uint8_t isCommonAnode = (SEVEN_SEGMENTS[sevSegName].type == SEVEN_SEGMENT_COMMON_ANODE);
             
        ret = GPIO_WritePin(&gpioSevenSegment[sevSegName][SEVEN_SEGMENTS[sevSegName].pins[8]], 1 ^ isCommonAnode);
        if(ret != STD_SUCCESS){
            ret = STD_ERROR;
        }
    }

    return ret;
}

STD_ReturnType SevenSegment_Clear(SEVEN_SEGMENT_Names_t sevSegName){
    STD_ReturnType ret = STD_SUCCESS;

    if(sevSegName >= SEVEN_SEGMENT_LEN){
        ret = STD_ERROR;
    }
    else{
        const uint8_t isCommonAnode = (SEVEN_SEGMENTS[sevSegName].type == SEVEN_SEGMENT_COMMON_ANODE);

        for(volatile uint8_t pin = 0; pin < SEVEN_SEGMENT_NUMBER_OF_SEGMENTS+1; pin++){
                        
            ret = GPIO_WritePin(&gpioSevenSegment[sevSegName][pin], 0 ^ isCommonAnode);
            if(ret != STD_SUCCESS){
                ret = STD_ERROR;
                break;
            }
        }
    }

    return ret;
}
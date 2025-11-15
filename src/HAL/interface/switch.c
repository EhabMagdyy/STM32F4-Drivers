
#include "interface/HAL/switch.h"
#include "interface/MCAL/gpio.h"

extern const SWITCH_Config_t SWITCHES[SWITCH_LEN];

GPIO_t gpio_switches[SWITCH_LEN];

STD_ReturnType SWITCH_Init(void){
    STD_ReturnType ret = STD_SUCCESS;
    for(uint8_t i = 0; i < SWITCH_LEN; i++){
        gpio_switches[i] = (GPIO_t){
            .port       = SWITCHES[i].port,
            .pin        = SWITCHES[i].pin,
            .mode       = GPIO_MODE_INPUT,
            .pullType   = (SWITCHES[i].resConfig == SWITCH_INTERNAL_PULLUP) ? GPIO_PULLUP :
                             (SWITCHES[i].resConfig == SWITCH_INTERNAL_PULLDOWN) ? GPIO_PULLDOWN :
                             GPIO_NOPULL,
            .altFunc    = GPIO_AF0_SYSTEM
        };

        ret = GPIO_Init(&gpio_switches[i]);
        if(ret != STD_SUCCESS){
            break;
        }
    }
    return ret;
}

STD_ReturnType SWITCH_ReadState(uint8_t switchName, Switch_State_t* state){
    STD_ReturnType ret = STD_SUCCESS;
    if(switchName >= SWITCH_LEN || state == NULL){
        ret = STD_ERROR;
    }
    else{
        GPIO_PinState_t gpioState;
        ret = GPIO_ReadPin(&gpio_switches[switchName], &gpioState);
        if(ret == STD_SUCCESS){
            for(volatile uint32_t j = 0; j < 10000; j++);
            ret = GPIO_ReadPin(&gpio_switches[switchName], &gpioState);
            if(ret == STD_SUCCESS){
                for(volatile uint32_t j = 0; j < 10000; j++);
            }
            if(gpioState == GPIO_PIN_SET){
                if(SWITCHES[switchName].resConfig == SWITCH_INTERNAL_PULLUP ||
                    SWITCHES[switchName].resConfig == SWITCH_EXTERNAL_PULLUP){
                    *state = SWITCH_RELEASED;
                }
                else{
                    *state = SWITCH_PRESSED;
                }
            }
            else{
                if(SWITCHES[switchName].resConfig == SWITCH_INTERNAL_PULLUP ||
                    SWITCHES[switchName].resConfig == SWITCH_EXTERNAL_PULLUP){
                    *state = SWITCH_PRESSED;
                }
                else{
                    *state = SWITCH_RELEASED;
                }
            }
        }
    }
    return ret;
}
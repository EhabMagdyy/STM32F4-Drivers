#include "interface/HAL/switch.h"
#include "interface/MCAL/gpio.h"
#include "OS/scheduler.h"

extern const SWITCH_Config_t SWITCHES[SWITCH_LEN];
static void SWITCH_Runnable(void* arg);

static Runnable__t SWITCH_RunnableObj = {
    .callback = SWITCH_Runnable,
    .Periodicity = 10,   // 10 ms
    .FirstDelay = 0,
    .arg = 0
};

GPIO_t gpio_switches[SWITCH_LEN];

// Debouncing structure
static uint8_t CurState[SWITCH_LEN];
static uint8_t PrevState[SWITCH_LEN];
static uint8_t SwitchCounter[SWITCH_LEN];
static Switch_State_t SwitchState[SWITCH_LEN];

STD_ReturnType SWITCH_Init(void){
    STD_ReturnType ret = STD_SUCCESS;

    for(uint8_t i = 0; i < SWITCH_LEN; i++){

        gpio_switches[i] = (GPIO_t){
            .port     = SWITCHES[i].port,
            .pin      = SWITCHES[i].pin,
            .mode     = GPIO_MODE_INPUT,
            .pullType = (SWITCHES[i].resConfig == SWITCH_INTERNAL_PULLUP) ? GPIO_PULLUP :
                        (SWITCHES[i].resConfig == SWITCH_INTERNAL_PULLDOWN) ? GPIO_PULLDOWN :
                         GPIO_NOPULL,
            .altFunc  = GPIO_AF0_SYSTEM
        };

        ret = GPIO_Init(&gpio_switches[i]);
        if(ret != STD_SUCCESS){ break; }

        // Initialize debouncing arrays
        PrevState[i] = 0;
        SwitchCounter[i] = 0;
        SwitchState[i] = SWITCH_RELEASED;
    }
    if(ret == STD_SUCCESS){
        ret = Scheduler_RegisterRunnable(&SWITCH_RunnableObj);
    }

    return ret;
}

STD_ReturnType SWITCH_ReadState(uint8_t switchName, Switch_State_t *state)
{
    if(switchName >= SWITCH_LEN || state == NULL)
        return STD_ERROR;

    *state = SwitchState[switchName];
    return STD_SUCCESS;
}

static void SWITCH_Runnable(void* arg){
    GPIO_PinState_t pinRaw;

    for(uint8_t i = 0; i < SWITCH_LEN; i++){
        // 1. Read current pin state
        GPIO_ReadPin(&gpio_switches[i], &pinRaw);
        // Convert to logical 1/0 based on pull config
        if(pinRaw == GPIO_PIN_SET){
            CurState[i] = (SWITCHES[i].resConfig == SWITCH_INTERNAL_PULLUP ||
                           SWITCHES[i].resConfig == SWITCH_EXTERNAL_PULLUP)
                          ? 0 : 1;   // pullup: SET means released
        }
        else{
            CurState[i] = (SWITCHES[i].resConfig == SWITCH_INTERNAL_PULLUP ||
                           SWITCHES[i].resConfig == SWITCH_EXTERNAL_PULLUP)
                          ? 1 : 0;   // pullup: RESET means pressed
        }

        // 2. Compare with previous reading
        if(CurState[i] == PrevState[i]){
            // Stable reading → increase stability counter
            SwitchCounter[i]++;

            if(SwitchCounter[i] >= 5){
                SwitchCounter[i] = 0;
                SwitchState[i] = CurState[i];
            }
        }
        else{
            // Changed reading -> reset counter
            SwitchCounter[i] = 0;
        }
        // 3. Store current as previous for next cycle
        PrevState[i] = CurState[i];
    }
}

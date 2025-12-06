#include "interface/HAL/keypad.h"
#include "OS/scheduler.h"

extern keypad_t Keypad[KEYPAD_LEN];
extern const uint8_t keypad_matrix[KEYPAD_ROWS][KEYPAD_COLOUMNS];

static void keypad_Runnable(void *arg);

// Register runnable
static Runnable__t KEYPAD_RunnableObj = {
    .callback     = keypad_Runnable,
    .Periodicity  = 5,  // scan every 5 ms
    .FirstDelay   = 0,
    .arg          = KEYPAD_0
};

/* State machine data */
volatile uint8_t currentRow = 0;
volatile uint8_t stableKey = 0;
volatile uint8_t lastKey = 0;
volatile uint8_t debounceCounter = 0;

STD_ReturnType keypad_Init(void)
{
    STD_ReturnType ret = STD_SUCCESS;

    /* Init rows as OUTPUT LOW */
    for(uint8_t r = 0 ; r < KEYPAD_ROWS ; r++){
        ret = GPIO_Init(&Keypad[KEYPAD_0].rowPins[r]);
        if(ret != STD_SUCCESS) return ret;
        GPIO_WritePin(&Keypad[KEYPAD_0].rowPins[r], GPIO_PIN_RESET);
    }

    /* Init columns as INPUT + pulldown */
    for(uint8_t c = 0 ; c < KEYPAD_COLOUMNS ; c++){
        ret = GPIO_Init(&Keypad[KEYPAD_0].colPins[c]);
        if(ret != STD_SUCCESS) return ret;
    }

    return Scheduler_RegisterRunnable(&KEYPAD_RunnableObj);
}

STD_ReturnType keypad_GetKey(uint8_t *key){
    if(key == NULL)
        return STD_ERROR;

    *key = stableKey;
    return STD_SUCCESS;
}

static uint8_t stableKeyCandidate = 0;

static void keypad_Runnable(void *arg)
{
    uint8_t pin_state = 0;
    uint8_t detectedKey = 0;

    /* Turn off all rows */
    for(uint8_t r = 0; r < KEYPAD_ROWS; r++)
        GPIO_WritePin(&Keypad[KEYPAD_0].rowPins[r], GPIO_PIN_RESET);

    /* Activate current row */
    GPIO_WritePin(&Keypad[KEYPAD_0].rowPins[currentRow], GPIO_PIN_SET);

    /* Scan columns */
    for(uint8_t c = 0; c < KEYPAD_COLOUMNS; c++){
        GPIO_ReadPin(&Keypad[KEYPAD_0].colPins[c], &pin_state);
        if(pin_state == GPIO_PIN_SET){
            detectedKey = keypad_matrix[currentRow][c];
            break;
        }
    }

    if(detectedKey != 0){
        stableKeyCandidate = detectedKey;
    }

    currentRow++;
    if(currentRow >= KEYPAD_ROWS){
        currentRow = 0;

        if(stableKeyCandidate == lastKey && stableKeyCandidate != 0){
            if(debounceCounter < 5) debounceCounter++;
            if(debounceCounter == 5)
                stableKey = stableKeyCandidate;
        }
        else{
            debounceCounter = 0;
            if(stableKeyCandidate == 0)
                stableKey = 0;
        }

        lastKey = stableKeyCandidate;
        stableKeyCandidate = 0;   // reset for next scan
    }
}

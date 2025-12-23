#include "private/MCAL/wdt_priv.h"
#include "interface/MCAL/wdt.h"
#include "interface/MCAL/rcc.h"

STD_ReturnType WDT_Init(WDT_Prescaler_t prescaler, uint16_t reloadValue){
    // Enable RCC LSI Clock
    RCC_SetLSI(RCC_ENABLE);
    // Enable access to WDT registers
    WDT->KR = WDT_KEY_ACCESS;
    // Set Prescaler
    WDT->PR = prescaler;
    // Set Reload Value
    WDT->RLR = reloadValue;
    // Start
    WDT_Start();
    // Refresh
    WDT_Refresh();

    return STD_SUCCESS;
}

STD_ReturnType WDT_Refresh(void){
    WDT->KR = WDT_KEY_RELOAD;

    return STD_SUCCESS;
}

STD_ReturnType WDT_Start(void){
    WDT->KR = WDT_KEY_START;

    return STD_SUCCESS;
}
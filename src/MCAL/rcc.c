
#include "/home/ehab/Documents/ITI_9Months/ARM/STM32F401_Drivers/include/interface/MCAL/rcc.h"
#include "/home/ehab/Documents/ITI_9Months/ARM/STM32F401_Drivers/include/private/MCAL/rcc_priv.h"

STD_ReturnType RCC_cfgClk(const RCC_CFG_t *cfg){
    STD_ReturnType ret = STD_SUCCESS;
    if(cfg == NULL){
        ret = STD_ERROR;
    }
    else{
        if(cfg->sysClkSource == RCC_CLOCK_SOURCE_HSI || cfg->sysClkSource == RCC_CLOCK_SOURCE_HSE){
            ret = RCC_setClk(cfg->sysClkSource, RCC_ENABLE);
            if(ret == STD_SUCCESS){
                ret = RCC_waitForClkReady(cfg->sysClkSource, 500);
            }
            else{
                // Do nothing
            }
            // Set AHB & APB Prescalers
            if(ret == STD_SUCCESS){
                ret = RCC_setAHBPrescaler(RCC_AHB_PRESCALER_DIV1);
            }
            else{
                // Do nothing
            }
            if(ret == STD_SUCCESS){
                ret = RCC_setAPBPrescaler(RCC_APB_PRESCALER_DIV1, RCC_APB1);
            }
            else{
                // Do nothing
            }
            if(ret == STD_SUCCESS){
                ret = RCC_setAPBPrescaler(RCC_APB_PRESCALER_DIV1, RCC_APB2);
            }
            else{
                // Do nothing
            }
            if(ret == STD_SUCCESS){
                ret = RCC_setSysClk(cfg->sysClkSource);
            }
            else{
                // Do nothing
            }
            if(ret == STD_SUCCESS){
                ret = RCC_waitForSysClkReady(cfg->sysClkSource, 500);
            }
            else{
                // Do nothing
            }
        }
        else if(cfg->sysClkSource == RCC_CLOCK_SOURCE_PLL){
            ret = RCC_setClk(cfg->pllClkSource, RCC_ENABLE);
            if(ret == STD_SUCCESS){
                ret = RCC_waitForClkReady(cfg->pllClkSource, 500);
            }
            else{
                // Do nothing
            }
            // set system clock to pll clock source
            if(ret == STD_SUCCESS){
                ret = RCC_setSysClk(cfg->pllClkSource);
            }
            else{
                // Do nothing
            }
            if(ret == STD_SUCCESS){
                ret = RCC_waitForSysClkReady(cfg->pllClkSource, 500);
            }
            else{
                // Do nothing
            }
            // Disable the PLL before configuring
            if(ret == STD_SUCCESS){
                ret = RCC_setClk(RCC_CLOCK_SOURCE_PLL, RCC_DISABLE);
            }
            else{
                // Do nothing
            }
            // Configure PLL
            if(ret == STD_SUCCESS){
                ret = RCC_setPLLClockSource(cfg->pllClkSource);
            }
            else{
                // Do nothing
            }
            // C. Set Flash Latency (CRITICAL STEP for 84 MHz)
            *(uint32_t*)((0x40000000UL + 0x00020000UL) + 0x3C00UL) &= ~0b111;
            *(uint32_t*)((0x40000000UL + 0x00020000UL) + 0x3C00UL) |= (2 & 0b111);

            if(ret == STD_SUCCESS){
                ret = RCC_pllCfg(&(cfg->pllConfig));
            }
            else{
                // Do nothing
            }
            // Enable PLL
            if(ret == STD_SUCCESS){
                ret = RCC_setClk(RCC_CLOCK_SOURCE_PLL, RCC_ENABLE);
            }
            else{
                // Do nothing
            }
            if(ret == STD_SUCCESS){
                ret = RCC_waitForClkReady(RCC_CLOCK_SOURCE_PLL, 500);
            }
            else{
                // Do nothing
            }
            // Set AHB & APB Prescalers
            if(ret == STD_SUCCESS){
                ret = RCC_setAHBPrescaler(RCC_AHB_PRESCALER_DIV1);
            }
            else{
                // Do nothing
            }
            if(ret == STD_SUCCESS){
                ret = RCC_setAPBPrescaler(RCC_APB_PRESCALER_DIV2, RCC_APB1);
            }
            else{
                // Do nothing
            }
            if(ret == STD_SUCCESS){
                ret = RCC_setAPBPrescaler(RCC_APB_PRESCALER_DIV1, RCC_APB2);
            }
            else{
                // Do nothing
            }
            // Set System Clock to PLL
            if(ret == STD_SUCCESS){
                ret = RCC_setSysClk(cfg->sysClkSource);
            }
            else{
                // Do nothing
            }
            if(ret == STD_SUCCESS){
                ret = RCC_waitForSysClkReady(cfg->sysClkSource, 500);
            }
            else{
                // Do nothing
            }
        }
        else {
            ret = STD_ERROR;
        }
    }
    return ret;
}

STD_ReturnType RCC_setSysClk(RCC_ClockType_t clockType){
    STD_ReturnType ret = STD_SUCCESS;
    switch (clockType){
        case RCC_CLOCK_SOURCE_HSI:
            RCC->CFGR.BITS.SW = 0b00;
            break;
        case RCC_CLOCK_SOURCE_HSE:
            RCC->CFGR.BITS.SW = 0b01;
            break;
        case RCC_CLOCK_SOURCE_PLL:
            RCC->CFGR.BITS.SW = 0b10;
            break;
        default:
            ret = STD_ERROR;
            break;
    }
    return ret;
}

STD_ReturnType RCC_waitForSysClkReady(RCC_ClockType_t clockType, uint32_t timeout){
    uint32_t tickStart = 0;
    STD_ReturnType ret = STD_SUCCESS;
    switch (clockType){
        case RCC_CLOCK_SOURCE_HSI:
            while ((RCC->CFGR.BITS.SWS != 0b00) && (tickStart < timeout)){
                tickStart++;
            }
            if(timeout == tickStart){
                ret = STD_TIMEOUT;
            }
            else{
                // Do nothing
            }
            break;
        case RCC_CLOCK_SOURCE_HSE:
            while ((RCC->CFGR.BITS.SWS != 0b01) && (tickStart < timeout)){
                tickStart++;
            }
            if(timeout == tickStart){
                ret = STD_TIMEOUT;
            }
            else{
                // Do nothing
            }
            break;
        case RCC_CLOCK_SOURCE_PLL:
            while ((RCC->CFGR.BITS.SWS != 0b10) && (tickStart < timeout)){
                tickStart++;
            }
            if(timeout == tickStart){
                ret = STD_TIMEOUT;
            }
            else{
                // Do nothing
            }
            break;
        default:
            ret = STD_ERROR;
            break;
    }
    return ret;
}

STD_ReturnType RCC_setClk(RCC_ClockType_t clockType, RCC_Clock_Status_t status){
    STD_ReturnType ret = STD_SUCCESS;
    switch (clockType){
        case RCC_CLOCK_SOURCE_HSI:
            RCC->CR.BITS.HSION = status;
            break;
        case RCC_CLOCK_SOURCE_HSE:
            RCC->CR.BITS.HSEON = status;
            break;
        case RCC_CLOCK_SOURCE_PLL:
            RCC->CR.BITS.PLLON = status;
            break;
        default:
            ret = STD_ERROR;
            break;
    }
    return ret;
}


STD_ReturnType RCC_waitForClkReady(RCC_ClockType_t clockType, uint32_t timeout){
    uint32_t tickStart = 0;
    STD_ReturnType ret = STD_SUCCESS;
    switch (clockType){
        case RCC_CLOCK_SOURCE_HSI:
            while ((RCC->CR.BITS.HSIRDY == 0) && (tickStart < timeout)){
                tickStart++;
            }
            if(timeout == tickStart){
                ret = STD_TIMEOUT;
            }
            else{
                // Do nothing
            }
            break;
        case RCC_CLOCK_SOURCE_HSE:
            while ((RCC->CR.BITS.HSERDY == 0) && (tickStart < timeout)){
                tickStart++;
            }
            if(timeout == tickStart){
                ret = STD_TIMEOUT;
            }
            else{
                // Do nothing
            }
            break;
        case RCC_CLOCK_SOURCE_PLL:
            while ((RCC->CR.BITS.PLLRDY == 0) && (tickStart < timeout)){
                tickStart++;
            }
            if(timeout == tickStart){
                ret = STD_TIMEOUT;
            }
            else{
                // Do nothing
            }
            break;
        default:
            ret = STD_ERROR;
            break;
    }
    return ret;
}

////////////////////// PLL Configurations /////////////////////    
STD_ReturnType RCC_pllCfg(const PLL_CFG_t *pllCfg){
    STD_ReturnType ret = STD_SUCCESS;
    
    if (pllCfg == NULL) {
        ret = STD_ERROR;
    }
    else {
        if (pllCfg->pll_cfg_max_t.pllMax == RCC_PLL_MAX) {
            ret = RCC_setPLLMaxClock();
        }
        else {
            RCC->PLLCFGR.BITS.PLLM = pllCfg->pll_cfg_custom_t.PLLM;
            RCC->PLLCFGR.BITS.PLLN = pllCfg->pll_cfg_custom_t.PLLN;
            switch (pllCfg->pll_cfg_custom_t.PLLP) {
                case 2:
                    RCC->PLLCFGR.BITS.PLLP = 0b00; // Division by 2
                    break;
                case 4:
                    RCC->PLLCFGR.BITS.PLLP = 0b01; // Division by 4
                    break;
                case 6:
                    RCC->PLLCFGR.BITS.PLLP = 0b10; // Division by 6
                    break;
                case 8:
                    RCC->PLLCFGR.BITS.PLLP = 0b11; // Division by 8
                    break;
                default:
                    ret = STD_ERROR; // Invalid PLLP value
                    break;
            }
            if (ret == STD_SUCCESS) {
                RCC->PLLCFGR.BITS.PLLQ = pllCfg->pll_cfg_custom_t.PLLQ;
            }
            else{
                // Do nothing
            }
        }
    }
    
    return ret;
}

STD_ReturnType RCC_setPLLClockSource(RCC_ClockType_t source){
    STD_ReturnType ret = STD_SUCCESS;
    switch (source){
        case RCC_CLOCK_SOURCE_HSI:
            RCC->PLLCFGR.BITS.PLLSRC = 0;
            break;
        case RCC_CLOCK_SOURCE_HSE:
            RCC->PLLCFGR.BITS.PLLSRC = 1;
            break;
        default:
            ret = STD_ERROR;
            break;
    }
    return ret;
}

STD_ReturnType RCC_checkPLLClockSource(RCC_ClockType_t *source){
    STD_ReturnType ret = STD_SUCCESS;
    if(source == NULL){
        ret = STD_ERROR;
        return ret;
    }
    else{
        if (RCC->PLLCFGR.BITS.PLLSRC == 0){
            *source = RCC_CLOCK_SOURCE_HSI;
        }
        else {
            *source = RCC_CLOCK_SOURCE_HSE;
        }
    }

    return ret;
}

STD_ReturnType RCC_setPLLMaxClock(void){
    STD_ReturnType ret = STD_SUCCESS;
    // Get the current PLL clock source HSI/HSE
    RCC_ClockType_t currentSource = RCC_CLOCK_SOURCE_HSI;
    ret = RCC_checkPLLClockSource(&currentSource);
    if(ret == STD_SUCCESS){
        if(currentSource == RCC_CLOCK_SOURCE_HSI){
            // Set the PLL to maximum clock settings for HSI (HSI=16MHz)
            RCC->PLLCFGR.BITS.PLLM = 8;  // 16/8 = 2MHz
            RCC->PLLCFGR.BITS.PLLN = 168;// 2*168 = 336MHz
            RCC->PLLCFGR.BITS.PLLP = 1;  // 336/4 = 84MHz
            RCC->PLLCFGR.BITS.PLLQ = 7;  // 336/7 = 48MHz
        }
        else {
            // Set the PLL to maximum clock settings for HSE (HSE=25MHz)
            RCC->PLLCFGR.BITS.PLLM = 25;  // 25/25 = 1 MHz
            RCC->PLLCFGR.BITS.PLLN = 336; // 1 * 336 = 336 MHz
            RCC->PLLCFGR.BITS.PLLP = 1;   // /4 = 84 MHz (00 -> /2, 01 -> /4)
            RCC->PLLCFGR.BITS.PLLQ = 7;   // /7 = 48 MHz (USB)
        }
    }
    else {
        /* STD_ERROR */
    }

    return ret;
}

//////////// Peripherals Configurations (Enable/Disable/Reset) //////////////  
static STD_ReturnType RCC_waitPeripheralReady(RCC_Peripheral_t peripheral, uint32_t timeout){
    STD_ReturnType ret = STD_SUCCESS;
    uint32_t tickStart = 0;
    uint32_t busID = (peripheral & RCC_BUS_MASK) >> RCC_BUS_OFFSET;  // 1/2/3/4
    
    switch (busID) {
        case RCC_AHB1: 
            while(((RCC->AHB1ENR.REG & peripheral) == 0) && tickStart < timeout){
                tickStart++;
            }
            if(timeout == tickStart){
                ret = STD_TIMEOUT;
            }
            else{
                // Do nothing
            }
            break;
        case RCC_AHB2: 
            while(((RCC->AHB2ENR.REG & peripheral) == 0) && tickStart < timeout){
                tickStart++;
            }
            if(timeout == tickStart){
                ret = STD_TIMEOUT;
            }
            else{
                // Do nothing
            }
            break;
        case RCC_APB1: 
            while(((RCC->APB1ENR.REG & peripheral) == 0) && tickStart < timeout){
                tickStart++;
            }
            if(timeout == tickStart){
                ret = STD_TIMEOUT;
            }
            else{
                // Do nothing
            }
            break;
        case RCC_APB2: 
            while(((RCC->APB2ENR.REG & peripheral) == 0) && tickStart < timeout){
                tickStart++;
            }
            if(timeout == tickStart){
                ret = STD_TIMEOUT;
            }
            else{
                // Do nothing
            }
            break;
        default:
            ret = STD_ERROR;
            break;
    }    

    return ret;
}

// Can send multpile parameters only for the same bus: RCC_GPIOA | RCC_GPIOC
STD_ReturnType RCC_ctrlPeripheral(RCC_Peripheral_t peripheral, RCC_Peripheral_Operation_t operation){
    STD_ReturnType ret = STD_SUCCESS;
    uint32_t busID = (peripheral & RCC_BUS_MASK) >> RCC_BUS_OFFSET;  // 1/2/3/4
    
    switch (operation) {
        case RCC_PERIPHERAL_ENABLE:
            switch (busID) {
                case RCC_AHB1: 
                    RCC->AHB1ENR.REG |= peripheral;
                    break;
                case RCC_AHB2: 
                    RCC->AHB2ENR.REG |= peripheral;
                    break;
                case RCC_APB1: 
                    RCC->APB1ENR.REG |= peripheral;
                    break;
                case RCC_APB2: 
                    RCC->APB2ENR.REG |= peripheral;
                    break;
                default:
                    ret = STD_ERROR;
                    break;
            }
            if(ret == STD_SUCCESS){
                ret = RCC_waitPeripheralReady(peripheral, 50);
            }
            else{
                // Do nothing
            }
            break;
        case RCC_PERIPHERAL_DISABLE:
            switch (busID) {
                case RCC_AHB1: 
                    RCC->AHB1ENR.REG &= ~peripheral;
                    break;
                case RCC_AHB2: 
                    RCC->AHB2ENR.REG &= ~peripheral;
                    break;
                case RCC_APB1: 
                    RCC->APB1ENR.REG &= ~peripheral;
                    break;
                case RCC_APB2: 
                    RCC->APB2ENR.REG &= ~peripheral;
                    break;
                default:
                    ret = STD_ERROR;
                    break;
            }
            if(ret == STD_SUCCESS){
                ret = RCC_waitPeripheralReady(peripheral, 50);
            }
            else{
                // Do nothing
            }
            break;
        case RCC_PERIPHERAL_RESET:
            switch (busID) {
                case RCC_AHB1: 
                    RCC->AHB1RSTR.REG |= peripheral;
                    RCC->AHB1RSTR.REG &= ~peripheral;
                    break;
                case RCC_AHB2: 
                    RCC->AHB2RSTR.REG |= peripheral;
                    RCC->AHB2RSTR.REG &= ~peripheral;
                    break;
                case RCC_APB1: 
                    RCC->APB1RSTR.REG |= peripheral;
                    RCC->APB1RSTR.REG &= ~peripheral;
                    break;
                case RCC_APB2: 
                    RCC->APB2RSTR.REG |= peripheral;
                    RCC->APB2RSTR.REG &= ~peripheral;
                    break;
                default:
                    ret = STD_ERROR;
                    break;
            }
            if(ret == STD_SUCCESS){
                ret = RCC_waitPeripheralReady(peripheral, 50);
            }
            else{
                // Do nothing
            }
            break;
        default:
            ret = STD_ERROR;
            break;
    }    

    return ret;
}

////////////////////////// Bus Prescalers ///////////////////////////
STD_ReturnType RCC_setAHBPrescaler(RCC_AHB_Prescaler_t prescaler){
    STD_ReturnType ret = STD_SUCCESS;
    RCC->CFGR.BITS.HPRE = prescaler;
    return ret;
}
STD_ReturnType RCC_setAPBPrescaler(RCC_APB_Prescaler_t prescaler, RCC_BusType_t bus){
    STD_ReturnType ret = STD_SUCCESS;
    switch (bus){
        case RCC_APB1:
            RCC->CFGR.BITS.PPRE1 = prescaler;
            break;
        case RCC_APB2:
            RCC->CFGR.BITS.PPRE2 = prescaler;
            break;
        default:
            ret = STD_ERROR;
            break;
    }
    return ret;
}

/////////////////// HSE Bypass ///////////////////////////
STD_ReturnType RCC_setHSEBypass(void){
    STD_ReturnType ret = STD_SUCCESS;
    RCC->CR.BITS.HSEBYP = 1;
    return ret;
}
STD_ReturnType RCC_clearHSEBypass(void){
    STD_ReturnType ret = STD_SUCCESS;
    RCC->CR.BITS.HSEBYP = 0;
    return ret;
}
/////////////////// Clock Security System ///////////////////
STD_ReturnType RCC_clockSecurityEnable(void){
    STD_ReturnType ret = STD_SUCCESS;
    RCC->CR.BITS.CSSON = 1;
    return ret;
}
STD_ReturnType RCC_clockSecurityDisable(void){
    STD_ReturnType ret = STD_SUCCESS;
    RCC->CR.BITS.CSSON = 0;
    return ret;
}
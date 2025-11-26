#include "private/Core/systick_priv.h"
#include "configuration/Core/systick_conf.h"
#include "interface/Core/systick.h"

static volatile FuncPtr SysTickHandler_CallBack = NULL;

volatile uint32_t SYSTICK_mode = SYSTICK_SINGLE_INTERVAL_MODE;

STD_ReturnType SYSTICK_Init(void){
	STD_ReturnType ret = STD_SUCCESS;

	SYSTICK_Stop();
	// Set Clock Source
	#if (SYSTICK_CLOCK_SOURCE == SYSTICK_CLOCK_SOURCE_PROCESSOR_DIV1)
		STK->CSR |= (1 << SYSTICK_CSR_CLKSOURCE_BIT_POS);
	#elif (SYSTICK_CLOCK_SOURCE == SYSTICK_CLOCK_SOURCE_PROCESSOR_DIV8)
		STK->CSR &= ~(1 << SYSTICK_CSR_CLKSOURCE_BIT_POS);
	#else
		#error "Invalid Clock Source"
	#endif
	STK->CSR &= ~(1 << SYSTICK_CSR_TICKINT_BIT_POS);	// Disable Exception Request

	return ret;
}

STD_ReturnType SYSTICK_Stop(void){
	STD_ReturnType ret = STD_SUCCESS;

	STK->CSR &= ~(1 << SYSTICK_CSR_ENABLE_BIT_POS);		// Disable
	STK->RVR = 0;										// Clear Reload Value
	STK->CVR = 0;										// Clear Current Value
	STK->CSR &= ~(1 << SYSTICK_CSR_TICKINT_BIT_POS);	// Disable Exception Request

	return ret;
}

STD_ReturnType SYSTICK_DelayMS(uint16_t delayMillieSec){
	STD_ReturnType ret = STD_SUCCESS;

	uint32_t reqReloadVal = (delayMillieSec * SYSTICK_CLOCK_FREQ_KHZ) - 1;
	if(reqReloadVal > SYSTICK_MAX_TICKS){
		ret = STD_ERROR;
	}
	else{
		STK->RVR = reqReloadVal;
		STK->CSR |= (1 << SYSTICK_CSR_ENABLE_BIT_POS);
		while(0 == ((STK->CSR >> SYSTICK_CSR_COUNTFLAG_BIT_POS) & 1U));
		SYSTICK_Stop();
	}

	return ret;
}

STD_ReturnType SYSTICK_DelayUS(uint32_t delayMicroSec){
    STD_ReturnType ret = STD_SUCCESS;
    uint32_t RVR_Value;

    uint32_t reqReloadVal = (delayMicroSec * (SYSTICK_CLOCK_FREQ_KHZ / 1000)) - 1;
	if(reqReloadVal > SYSTICK_MAX_TICKS){
		ret = STD_ERROR;
	}
	else{
        RVR_Value = reqReloadVal;
		STK->RVR = RVR_Value;
        STK->CSR |= (1 << SYSTICK_CSR_ENABLE_BIT_POS);
        while(0 == ((STK->CSR >> SYSTICK_CSR_COUNTFLAG_BIT_POS) & 1U));
        
        SYSTICK_Stop();
    }

    return ret;
}

STD_ReturnType SYSTICK_SingleInterval(uint16_t delayMillieSec, FuncPtr SysTickHandler_CB){
	STD_ReturnType ret = STD_SUCCESS;

	if(NULL == SysTickHandler_CB){
		ret = STD_ERROR;
	}
	else{
		uint32_t reqReloadVal = (delayMillieSec * SYSTICK_CLOCK_FREQ_KHZ) - 1;
		if(reqReloadVal > SYSTICK_MAX_TICKS){
			ret = STD_ERROR;
		}
		else{
			STK->RVR = reqReloadVal;
			SysTickHandler_CallBack = SysTickHandler_CB;
			STK->CSR |= (1 << SYSTICK_CSR_TICKINT_BIT_POS);
			SYSTICK_mode = SYSTICK_SINGLE_INTERVAL_MODE;		
			STK->CSR |= (1 << SYSTICK_CSR_ENABLE_BIT_POS);
		}
	}

	return ret;
}

STD_ReturnType SYSTICK_PeriodicInterval(uint16_t delayMillieSec, FuncPtr SysTickHandler_CB){
	STD_ReturnType ret = STD_SUCCESS;

	if(NULL == SysTickHandler_CB){
		ret = STD_ERROR;
	}
	else{ 
		uint32_t reqReloadVal = (delayMillieSec * SYSTICK_CLOCK_FREQ_KHZ) - 1;
		if(reqReloadVal > SYSTICK_MAX_TICKS){
			ret = STD_ERROR;
		}
		else{
			STK->RVR = reqReloadVal;
			SysTickHandler_CallBack = SysTickHandler_CB;
			STK->CSR |= (1 << SYSTICK_CSR_TICKINT_BIT_POS);
			SYSTICK_mode = SYSTICK_PERIODIC_INTERVAL_MODE;
			STK->CSR |= (1 << SYSTICK_CSR_ENABLE_BIT_POS);
		}
	}

	return ret;
}

STD_ReturnType SYSTICK_GetRemainingTicks(uint32_t* remTicks){
	STD_ReturnType ret = STD_SUCCESS;

	if(remTicks == NULL){
		ret = STD_ERROR;
	}
	else{
		*remTicks = STK->CVR;
	}

	return ret;
}

STD_ReturnType SYSTICK_GetElapsedTicks(uint32_t* elapsedTicks){
	STD_ReturnType ret = STD_SUCCESS;

	if(elapsedTicks == NULL){
		ret = STD_ERROR;
	}
	else{
		*elapsedTicks = STK->RVR - STK->CVR;
	}

	return ret;
}

// Systick ISR
void SysTick_Handler(void){
	if(SYSTICK_mode == SYSTICK_SINGLE_INTERVAL_MODE){
		SYSTICK_Stop();
	}
	else{
		// Do Nothing
	}
	if(NULL != SysTickHandler_CallBack){
		SysTickHandler_CallBack();
	}
	else{
		// Do Nothing
	}
}

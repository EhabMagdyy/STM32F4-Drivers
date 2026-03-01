#include "private/MCAL/adc_priv.h"
#include "interface/MCAL/adc.h"
#include "interface/MCAL/rcc.h"
#include "interface/MCAL/gpio.h"

STD_ReturnType ADC_Init(ADC_t* config){
    if(config == NULL){
        return STD_ERROR;
    }
    // Enable RCC for ADC peripheral
    STD_ReturnType status = RCC_ControlPeripheral(RCC_ADC1, RCC_PERIPHERAL_ENABLE);
    if(status != STD_SUCCESS){
        return status;
    }

    // Set Resolution
    ADC->CR1 |= (config->resolution << 24);

    // Set Channel
    ADC->SQR1 = (config->seqLength << 20);  // Set regular sequence length
    ADC->SQR3 = (config->channel & 0x1F);   // Set first conversion in regular sequence to the specified channel

    // Set Sample Time
    uint32_t sampleTimeBits = (config->sampleTime & 0x7) << (config->channel * 3);
    if(config->channel < 10){
        ADC->SMPR2 |= sampleTimeBits; // Sample time for channels 0-9
    } 
    else {
        ADC->SMPR1 |= sampleTimeBits; // Sample time for channels 10-15
    }

    // Configure GPIO pin for ADC Channel
    uint8_t gpioPortChannel[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2}; // 0 > GPIOA, 1 > GPIOB, 2 > GPIOC
    uint8_t gpioPinChannel[16] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 0, 1, 2, 3, 4, 5};  // Corresponding pins for ADC channels
    GPIO_t pinConfig = (GPIO_t){
        .port = gpioPortChannel[config->channel],
        .pin = gpioPinChannel[config->channel],
        .mode = GPIO_MODE_ANALOG,
        .pullType = GPIO_NOPULL,
        .speed = GPIO_SPEED_LOW
    };
    GPIO_Init(&pinConfig);

    // Enable ADC
    ADC->CR2 |= (1 << 0); // ADON bit

    return STD_SUCCESS;
}

STD_ReturnType ADC_SingleRead(ADC_t* config, uint16_t* value){
    if(config == NULL || value == NULL){
        return STD_ERROR;
    }

    // Start conversion
    ADC->CR2 |= (1 << 30); // SWSTART bit

    // Wait for conversion to complete
    while((ADC->SR & (1 << 1)) == 0); // Wait for EOC bit

    *value = ADC->DR & 0xFFFF; // Read the result from data register

    return STD_SUCCESS;
}
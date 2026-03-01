#include "private/MCAL/adc_priv.h"
#include "interface/MCAL/adc.h"
#include "interface/MCAL/rcc.h"
#include "interface/MCAL/gpio.h"
#include "interface/Core/nvic.h"

volatile uint8_t adcNoOfReadings = 0;
volatile uint16_t* adcBuffer = NULL;
volatile ADC_Callback_t adcCallback = NULL;
volatile uint8_t isADCBusy = 0;

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

    // Set Sample Time
    for(uint8_t channelIdx = 0; channelIdx < config->numChannels; channelIdx++){
        uint32_t sampleTimeBits = (config->sampleTime & 0x7) << (config->channels[channelIdx] * 3);
        if(config->channels[channelIdx] < 10){
            ADC->SMPR2 |= sampleTimeBits; // Sample time for channels 0-9
        } 
        else {
            ADC->SMPR1 |= sampleTimeBits; // Sample time for channels 10-15
        }
    }

    // Configure GPIO pin for ADC Channel
    uint8_t gpioPortChannel[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2}; // 0 > GPIOA, 1 > GPIOB, 2 > GPIOC
    uint8_t gpioPinChannel[16] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 0, 1, 2, 3, 4, 5};  // Corresponding pins for ADC channels
    for(uint8_t channelIdx = 0; channelIdx < config->numChannels; channelIdx++){
        GPIO_t pinConfig = (GPIO_t){
            .port = gpioPortChannel[config->channels[channelIdx]],
            .pin = gpioPinChannel[config->channels[channelIdx]],
            .mode = GPIO_MODE_ANALOG,
            .pullType = GPIO_NOPULL,
            .speed = GPIO_SPEED_LOW
        };
        GPIO_Init(&pinConfig);
    }

    // Set callback
    adcCallback = config->callback;

    // Enable ADC
    ADC->CR2 |= (1 << 0); // ADON bit

    return STD_SUCCESS;
}

STD_ReturnType ADC_SingleRead(ADC_Channel_t channel, uint16_t* value){
    if(value == NULL){
        return STD_ERROR;
    }

    // Select which channel you’re about to convert
    ADC->SQR3 = (channel & 0x1F);   // Set first conversion in regular sequence to the specified channel,
                                    // adc will will go to this channel first when conversion starts,
                                    // if seqLength > 1, it will continue to next channels in SQR3, SQR2, SQR1

    // Start conversion
    ADC->CR2 |= (1 << 30); // SWSTART bit

    // Wait for conversion to complete
    while((ADC->SR & (1 << 1)) == 0); // Wait for EOC bit

    *value = ADC->DR & 0xFFFF; // Read the result from data register

    return STD_SUCCESS;
}

STD_ReturnType ADC_ContinousRead(ADC_Channel_t channel, uint16_t* buffer, uint8_t noOfReadings){
    if(buffer == NULL || noOfReadings == 0){
        return STD_ERROR;
    }

    // Select which channel you’re about to convert
    ADC->SQR3 = (channel & 0x1F); 

    // Flush stale EOC/DR from previous run
    if (ADC->SR & (1 << 1)) {
        volatile uint32_t dummy = ADC->DR;
        (void)dummy;
    }

    // Enable continuous mode
    ADC->CR2 |= (1 << 1);     // CONT

    // Start conversion
    ADC->CR2 |= (1 << 30);    // SWSTART

    while(noOfReadings--){
        // Wait for conversion to complete
        while((ADC->SR & (1 << 1)) == 0);    // Wait for EOC bit
        *buffer++ = ADC->DR & 0xFFFF;        // Read the result from data register
    }

    ADC->CR2 &= ~(1 << 1);    // Disable CONT
    ADC->CR2 &= ~(1 << 30);   // Stop ADC conversions

    return STD_SUCCESS;
}

// Not reccommended in polling mode, better to use with DMA,
// because channel readings may overwrite each other in DR if the CPU doesn't read them fast enough.
STD_ReturnType ADC_ScanModeRead(ADC_Channel_t* channels, ADC_SeqLength_t seqLength, uint16_t* buffer, uint8_t noOfLoops){
    if(channels == NULL || buffer == NULL || noOfLoops == 0){
        return STD_ERROR;
    }

    uint8_t conversions = seqLength + 1;

    ADC->SQR1 &= ~0x3FFFFFFF;     // Clear SQ13..SQ16 & LEN bits
    ADC->SQR2 &= ~0x3FFFFFFF;     // Clear SQ7..SQ12
    ADC->SQR3 &= ~0x3FFFFFFF;     // Clear SQ1..SQ6

    // Configure the sequence length
    ADC->SQR1 |= (seqLength << 20);

    // Configure channel order
    for(uint8_t order = 0; order < conversions; order++){
        uint8_t channel = channels[order] & 0x1F;

        if(order < 6){
            ADC->SQR3 |= channel << (order * 5);
        } 
        else if(order < 12){
            ADC->SQR2 |= channel << ((order - 6) * 5);
        } 
        else {
            ADC->SQR1 |= channel << ((order - 12) * 5);
        }
    }

    // Flush stale EOC/DR from previous run
    if (ADC->SR & (1 << 1)) {
        volatile uint32_t dummy = ADC->DR;
        (void)dummy;
    }

    // Enable scan mode
    ADC->CR1 |= (1 << 8);   // SCAN

    // Enable continuous mode
    ADC->CR2 |= (1 << 1);   // CONT

    // Start conversion
    ADC->CR2 |= (1 << 30);  // SWSTART

    for(uint16_t i = 0; i < noOfLoops * conversions; i++){
        while(!(ADC->SR & (1 << 1)));  // EOC
        buffer[i] = ADC->DR;
    }

    ADC->CR1 &= ~(1 << 8);    // Stop scan mode
    ADC->CR2 &= ~(1 << 1);    // Disable CONT
    ADC->CR2 &= ~(1 << 30);   // Stop ADC conversions

    return STD_SUCCESS;
}

STD_ReturnType ADC_SingleReadIT(ADC_Channel_t channel, uint16_t* buffer){
    if(buffer == NULL || isADCBusy){
        return STD_ERROR;
    }

    isADCBusy = 1;
    adcNoOfReadings = 1;
    adcBuffer = buffer;
    
    // Select which channel you’re about to convert
    ADC->SQR3 = (channel & 0x1F);

    // Enable interrupt
    ADC->CR1 |= (1 << 5); // EOCIE

    // Enable NVIC for ADC
    NVIC_EnableIRQ(ADC_IRQn);

    // Start conversion
    ADC->CR2 |= (1 << 30); // SWSTART bit

    return STD_SUCCESS;
}

STD_ReturnType ADC_ContinousReadIT(ADC_Channel_t channel, uint16_t* buffer, uint8_t noOfReadings){
    if(buffer == NULL || noOfReadings == 0 || isADCBusy){
        return STD_ERROR;
    }

    isADCBusy = 1;
    adcNoOfReadings = noOfReadings;
    adcBuffer = buffer;

    // Select which channel you’re about to convert
    ADC->SQR3 = (channel & 0x1F);

    // Enable interrupt
    ADC->CR1 |= (1 << 5); // EOCIE

    // Enable NVIC for ADC
    NVIC_EnableIRQ(ADC_IRQn);

    // Enable continuous mode
    ADC->CR2 |= (1 << 1);   // CONT

    // Start conversion
    ADC->CR2 |= (1 << 30);  // SWSTART bit

    return STD_SUCCESS;
}

void ADC_IRQHandler(void){
    if(ADC->SR & (1 << 1)){ // Check for EOC interrupt
        uint16_t adcValue = ADC->DR & 0xFFFF;
        if(adcNoOfReadings > 0){
            *adcBuffer++ = adcValue;
            adcNoOfReadings--;
        }
        if(adcNoOfReadings == 0){
            // Disable EOC interrupt
            ADC->CR1 &= ~(1 << 5);
            isADCBusy = 0;
            // Disable NVIC for ADC
            NVIC_DisableIRQ(ADC_IRQn);
            // Call callback function
            if(adcCallback){
                adcCallback();
            }
        }
    }
}
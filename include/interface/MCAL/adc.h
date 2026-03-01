#ifndef ADC_H
#define ADC_H

#include "../../../lib/STD_Types.h"

typedef enum {
    ADC_CHANNEL_0 = 0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,
    ADC_CHANNEL_8,
    ADC_CHANNEL_9,
    ADC_CHANNEL_10,
    ADC_CHANNEL_11,
    ADC_CHANNEL_12,
    ADC_CHANNEL_13,
    ADC_CHANNEL_14,
    ADC_CHANNEL_15
} ADC_Channel_t;

typedef enum {
    ADC_RESOLUTION_12BIT = 0,
    ADC_RESOLUTION_10BIT,
    ADC_RESOLUTION_8BIT,
    ADC_RESOLUTION_6BIT
} ADC_Resolution_t;                 // Higher resolution provides more accuracy but takes longer time to convert and more power

typedef enum {
    ADC_SAMPLETIME_3CYCLES = 0,     // Shortest sample time for high-speed conversions but less accurate
    ADC_SAMPLETIME_15CYCLES,
    ADC_SAMPLETIME_28CYCLES,
    ADC_SAMPLETIME_56CYCLES,
    ADC_SAMPLETIME_84CYCLES,
    ADC_SAMPLETIME_112CYCLES,
    ADC_SAMPLETIME_144CYCLES,
    ADC_SAMPLETIME_480CYCLES        // Longest sample time for maximum accuracy but slower conversions
} ADC_SampleTime_t;                 // t_total = t_sample + t_conversion

// sequence length
typedef enum {
    ADC_SEQ_LENGTH_1 = 0,
    ADC_SEQ_LENGTH_2,
    ADC_SEQ_LENGTH_3,
    ADC_SEQ_LENGTH_4,
    ADC_SEQ_LENGTH_5,
    ADC_SEQ_LENGTH_6,
    ADC_SEQ_LENGTH_7,
    ADC_SEQ_LENGTH_8,
    ADC_SEQ_LENGTH_9,
    ADC_SEQ_LENGTH_10,
    ADC_SEQ_LENGTH_11,
    ADC_SEQ_LENGTH_12,
    ADC_SEQ_LENGTH_13,
    ADC_SEQ_LENGTH_14,
    ADC_SEQ_LENGTH_15,
    ADC_SEQ_LENGTH_16
} ADC_SeqLength_t;

typedef struct {
    ADC_Channel_t channel;
    ADC_Resolution_t resolution;
    ADC_SampleTime_t sampleTime;
    ADC_SeqLength_t seqLength;
} ADC_t;

STD_ReturnType ADC_Init(ADC_t* config);
STD_ReturnType ADC_SingleRead(ADC_t* config, uint16_t* value);

#endif /* ADC_H */
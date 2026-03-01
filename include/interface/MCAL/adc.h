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

typedef void (*ADC_Callback_t)(void);

typedef struct {
    ADC_Channel_t* channels;
    uint8_t numChannels;
    ADC_Resolution_t resolution;
    ADC_SampleTime_t sampleTime;
    ADC_Callback_t callback;
} ADC_t;

/**
 * @brief  Initializes the ADC peripheral with the specified configuration.
 * @param  config: Pointer to an ADC_t structure containing the desired ADC configuration.
 * @retval STD_SUCCESS if the ADC was initialized successfully, otherwise STD_ERROR.
 */
STD_ReturnType ADC_Init(ADC_t* config);
/**
 * @brief  Performs a single ADC conversion and retrieves the converted value.
 * @param  channel: The ADC channel to convert.
 * @param  value: Pointer to a uint16_t variable where the converted ADC value will be stored.
 * @retval STD_SUCCESS if the conversion was successful, otherwise STD_ERROR.
 */
STD_ReturnType ADC_SingleRead(ADC_Channel_t channel, uint16_t* value);
/**
 * @brief  Performs continuous ADC conversions and retrieves the converted values into an array.
 * @param  channel: The ADC channel to convert.
 * @param  buffer: Pointer to a uint16_t array where the converted ADC values will be stored.
 * @param  noOfReadings: The number of ADC conversions to perform and store in the array.
 * @retval STD_SUCCESS if the conversions were successful, otherwise STD_ERROR.
 */
STD_ReturnType ADC_ContinousRead(ADC_Channel_t channel, uint16_t* buffer, uint8_t noOfReadings);
/**
 * @brief  Performs ADC conversions in scan mode for multiple channels and retrieves the converted values into an array.
 * @param  channels: array of the channels in the sequence
 * @param  seqLength: The number of channels in the sequence.
 * @param  buffer: Pointer to a uint16_t array where the converted ADC values will be stored.
 * @param  noOfLoops: The number of ADC loops on the entire sequence to perform and store in the array.
 * @retval STD_SUCCESS if the conversions were successful, otherwise STD_ERROR.
 */
STD_ReturnType ADC_ScanModeRead(ADC_Channel_t* channels, ADC_SeqLength_t seqLength, uint16_t* buffer, uint8_t noOfLoops);

/**
 * @brief  Performs a single ADC conversion and retrieves the converted value.
 * @param  channel: The ADC channel to convert.
 * @param  value: Pointer to a uint16_t variable where the converted ADC value will be stored.
 * @retval STD_SUCCESS if the conversion was successful, otherwise STD_ERROR.
 */
STD_ReturnType ADC_SingleReadIT(ADC_Channel_t channel, uint16_t* value);
/**
 * @brief  Performs continuous ADC conversions and retrieves the converted values into an array.
 * @param  channel: The ADC channel to convert.
 * @param  buffer: Pointer to a uint16_t array where the converted ADC values will be stored.
 * @param  noOfReadings: The number of ADC conversions to perform and store in the array.
 * @retval STD_SUCCESS if the conversions were successful, otherwise STD_ERROR.
 */
STD_ReturnType ADC_ContinousReadIT(ADC_Channel_t channel, uint16_t* buffer, uint8_t noOfReadings);

#endif /* ADC_H */
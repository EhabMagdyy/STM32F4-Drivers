#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H

#include "../../../lib/STD_Types.h"
#include "configuration/HAL/sevenseg_cfg.h"

#define SEVEN_SEGMENT_MAX_NUMBER            9       // 0 - 1 - ... - 9
#define SEVEN_SEGMENT_NUMBER_OF_SEGMENTS    7       // Does not include Dot

typedef enum {
    SEVEN_SEGMENT_COMMON_CATHODE = 0,
    SEVEN_SEGMENT_COMMON_ANODE
} SEVENN_SEGMENT_TYPE_t;

typedef enum {
    SEVEN_SEGMENT_DOT_OFF = 0,
    SEVEN_SEGMENT_DOT_ON
} SEVEN_SEGMENT_Dot_State_t;

typedef struct {
    uint8_t port;           
    uint8_t pins[SEVEN_SEGMENT_NUMBER_OF_SEGMENTS+2];   // 0-6: Segments A-G, 7: Dot, 8: Common Pin                   
    SEVENN_SEGMENT_TYPE_t type;
} SEVEN_SEGMENT_Config_t;

/**
 * @brief Initializes all configured seven-segment display GPIO pins.
 * @return STD_ReturnType: STD_SUCCESS if initialization is successful, STD_ERROR otherwise.
 */
STD_ReturnType SevenSegment_Init(void);

/**
 * @brief De-initializes all configured seven-segment display GPIO pins.
 * @return STD_ReturnType: STD_SUCCESS if de-initialization is successful, STD_ERROR otherwise.
 */
STD_ReturnType SevenSegment_DeInit(void);

/**
 * @brief Writes a single digit (0-9) to a specified seven-segment display.
 * @param sevSegName: The name (index) of the display to write to.
 * @param number: The digit (0 to 9) to display.
 * @return STD_ReturnType: STD_SUCCESS if successful, STD_ERROR if name is invalid or number > 9.
 */
STD_ReturnType SevenSegment_Write(SEVEN_SEGMENT_Names_t sevSegName, uint8_t number);

/**
 * @brief Sets the dot state (ON or OFF) of the decimal point (dot) for a specified display.
 * @param sevSegName: The name (index) of the display.
 * @param state: The desired state of the dot (SEVENSEG_DOT_ON or SEVENSEG_DOT_OFF).
 * @return STD_ReturnType: STD_SUCCESS if successful, STD_ERROR otherwise.
 */
STD_ReturnType SevenSegment_SetDotState(SEVEN_SEGMENT_Names_t sevSegName, SEVEN_SEGMENT_Dot_State_t state);

/**
 * @brief Enables the specified seven-segment display.
 * @param sevSegName: The name (index) of the display to enable.
 * @return STD_ReturnType: STD_SUCCESS if successful, STD_ERROR otherwise.
 */
STD_ReturnType SevenSegment_Enable(SEVEN_SEGMENT_Names_t sevSegName);

/**
 * @brief Disables the specified seven-segment display.
 * @param sevSegName: The name (index) of the display to disable.
 * @return STD_ReturnType: STD_SUCCESS if successful, STD_ERROR otherwise.
 */
STD_ReturnType SevenSegment_Disable(SEVEN_SEGMENT_Names_t sevSegName);

/**
 * @brief Clears (turns off all segments) the specified seven-segment display.
 * @param sevSegName: The name (index) of the display to clear.
 * @return STD_ReturnType: STD_SUCCESS if successful, STD_ERROR otherwise.
 */
STD_ReturnType SevenSegment_Clear(SEVEN_SEGMENT_Names_t sevSegName);


#endif // SEVEN_SEGMENT_H
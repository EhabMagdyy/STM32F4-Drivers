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
    uint8_t pins[8];                              
    SEVENN_SEGMENT_TYPE_t type;
} SEVEN_SEGMENT_Config_t;

/**
 * @brief Initializes all configured seven-segment display GPIO pins.
 * * This function iterates through all configured displays and their 8 segments (a-g, DP)
 * and initializes the corresponding GPIO pins based on the hardware configuration 
 * (port, pin, mode, speed, etc.).
 *
 * @return STD_ReturnType: STD_SUCCESS if initialization is successful, STD_ERROR otherwise.
 */
STD_ReturnType SevenSegment_Init(void);

/**
 * @brief De-initializes all configured seven-segment display GPIO pins.
 * * This function resets the state of all GPIO pins used by the seven-segment displays, 
 * typically returning them to their default input state to save power.
 *
 * @return STD_ReturnType: STD_SUCCESS if de-initialization is successful, STD_ERROR otherwise.
 */
STD_ReturnType SevenSegment_DeInit(void);

/**
 * @brief Writes a single digit (0-9) to a specified seven-segment display.
 * * The function uses a common cathode pattern array and inverts the output 
 * state if the display is configured as common anode, ensuring compatibility 
 * with both display types.
 *
 * @param sevSegName: The name (index) of the display to write to.
 * @param number: The digit (0 to 9) to display.
 * @return STD_ReturnType: STD_SUCCESS if successful, STD_ERROR if name is invalid or number > 9.
 */
STD_ReturnType SevenSegment_Write(SEVEN_SEGMENT_Names_t sevSegName, uint8_t number);

/**
 * @brief Sets the dot state (ON or OFF) of the decimal point (dot) for a specified display.
 *
 * This function specifically targets the eighth segment (index 7) which is assumed
 * to be the Decimal Point (DP) pin.
 *
 * @param sevSegName: The name (index) of the display.
 * @param state: The desired state of the dot (SEVENSEG_DOT_ON or SEVENSEG_DOT_OFF).
 * @return STD_ReturnType: STD_SUCCESS if successful, STD_ERROR otherwise.
 */
STD_ReturnType SevenSegment_SetDotState(SEVEN_SEGMENT_Names_t sevSegName, SEVEN_SEGMENT_Dot_State_t state);

/**
 * @brief Clears (turns off all segments) the specified seven-segment display.
 *
 * This function effectively writes a 'blank' pattern (all segments OFF).
 *
 * @param sevSegName: The name (index) of the display to clear.
 * @return STD_ReturnType: STD_SUCCESS if successful, STD_ERROR otherwise.
 */
STD_ReturnType SevenSegment_Clear(SEVEN_SEGMENT_Names_t sevSegName);


#endif // SEVEN_SEGMENT_H
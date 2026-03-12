/******************************************************************************
 * @file <Filename>.h
 *
 * @author <Author>
 * @date   <Date>
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief <Some short descrition>
 *
 * @details <A more detailed description>
 *
 *
 *****************************************************************************/
#ifndef _LEDHANDLER_H_
#define _LEDHANDLER_H_


/***** INCLUDES **************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "LEDModule.h"

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/
#define LH_ERR_OK 0

/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/
/**
 * @brief Turns all LEDs off.
 */
void LEDHandler_AllOff(void);

/**
 * @brief Controls LEDs in operational mode.
 *
 * LED0 indicates operational mode.
 * LED1 indicates warning state.
 *
 * @param warning true if a warning condition is active
 */
void LEDHandler_OperationalMode(bool warning);

/**
 * @brief Controls LEDs in emergency mode.
 *
 * LED1 flashes to indicate an emergency alarm.
 */
void LEDHandler_EmergencyMode(void);

/**
 * @brief Controls LEDs in test mode.
 *
 * LED3 is used as test mode indicator.
 */
void LEDHandler_TestMode(void);
/**
 * @brief Controls LEDs in failure mode.
 *
 * LED2 indicates a general system failure.
 * LED4 indicates a sensor failure if active.
 *
 * @param sensorFailure true if the failure was caused by a sensor defect
 */
void LEDHandler_FailureMode(bool sensorFailure);


int32_t LEDHandler_Init(void);
#endif

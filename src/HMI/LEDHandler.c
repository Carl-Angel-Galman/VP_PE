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


/***** INCLUDES **************************************************************/
#include "LEDHandler.h"
#include "Application.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/


/***** PRIVATE FUNCTIONS *****************************************************/


void LEDHandler_AllOff(void)
{
    ledSetLED(LED0, LED_OFF);
    ledSetLED(LED1, LED_OFF);
    ledSetLED(LED2, LED_OFF);
    ledSetLED(LED3, LED_OFF);
    ledSetLED(LED4, LED_OFF);
}
void LEDHandler_OperationalMode(bool warning)
{
    ledSetLED(LED0, LED_ON);   // Operational indicator

    if(warning)
        ledSetLED(LED1, LED_ON);
    else
        ledSetLED(LED1, LED_OFF);

    ledSetLED(LED2, LED_OFF);
    ledSetLED(LED3, LED_OFF);
    ledSetLED(LED4, LED_OFF);
}
void LEDHandler_EmergencyMode(void)
{
    ledSetLED(LED0, LED_OFF);
    ledToggleLED(LED1);   // flashing alarm
    ledSetLED(LED2, LED_OFF);
    ledSetLED(LED3, LED_OFF);
    ledSetLED(LED4, LED_OFF);
}
void LEDHandler_TestMode(void)
{
    ledSetLED(LED0, LED_OFF);
    ledSetLED(LED1, LED_OFF);
    ledSetLED(LED2, LED_OFF);
    ledSetLED(LED3, LED_ON); // test mode indicator
    ledSetLED(LED4, LED_OFF);
}

void LEDHandler_FailureMode(bool sensorFailure)
{
    ledSetLED(LED0, LED_OFF);
    ledSetLED(LED1, LED_OFF);
    ledSetLED(LED2, LED_ON);   // system failure indicator
    ledSetLED(LED3, LED_OFF);

    if(sensorFailure)
        ledSetLED(LED4, LED_ON);
    else
        ledSetLED(LED4, LED_OFF);
}


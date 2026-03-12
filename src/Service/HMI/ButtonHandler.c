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
#include "ButtonHandler.h"
#include "stm32g4xx_hal.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/
#define DEBOUNCE_TIME 50

#define NUM_BUTTONS 3u

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/

static uint32_t lastDebounceTime[NUM_BUTTONS] = {};
static Button_Status_t lastButtonStates[NUM_BUTTONS] = {BUTTON_RELEASED,BUTTON_RELEASED,BUTTON_RELEASED};
static Button_Status_t currentButtonStates[NUM_BUTTONS] = {BUTTON_RELEASED,BUTTON_RELEASED,BUTTON_RELEASED};


/***** PUBLIC FUNCTIONS ******************************************************/
bool ButtonHandlerhasDebounced(Button_t button)
{

	Button_Status_t reading = buttonGetButtonStatus(button);
	if(reading != lastButtonStates[button])
	{
		lastDebounceTime[button] = HAL_GetTick();
	}
	if(HAL_GetTick() - lastDebounceTime[button] > DEBOUNCE_TIME)
	{
		if(reading != currentButtonStates[button])
		{
			currentButtonStates[button] = reading;
			if(currentButtonStates[button] == BUTTON_PRESSED)
			{
				return true;
			}
		}
	}
	lastButtonStates[button] = reading;
	return false;
}

int32_t ButtonHandlerInit(void){
	int32_t checkButtonInit = buttonInitialize();
	if(checkButtonInit != BUTTON_ERR_OK)
		return checkButtonInit;
	return BH_ERR_OK;
}

/***** PRIVATE FUNCTIONS *****************************************************/

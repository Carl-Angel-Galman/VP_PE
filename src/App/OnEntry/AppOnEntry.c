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

#include "AppOnEntry.h"
#include "AppContext.h"
#include <stddef.h>
#include "HMI/DisplayHandler.h"
#include "HMI/LEDHandler.h"
#include "HMI/ButtonHandler.h"
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/
static ApplicationContext_t *context = NULL;

/***** PUBLIC FUNCTIONS ******************************************************/
int32_t AppOnEntry_Init(void)
{
	context = AppContext_Get();
	if(context == NULL)
	{
		return APP_ON_ENTRY_ERR_INVALID_PTR;
	}
	return APP_ON_ENTRY_OK;

}



 int32_t initOnEntry(State_t* pState, int32_t eventID)
{

	LEDHandler_AllOff();

	return STATETBL_ERR_OK;
}


 int32_t displayDashOnEntry(State_t *pState, int32_t eventID)
{

	DisplayHandlerSetToIdle();

	return STATETBL_ERR_OK;
}

 int32_t preOperationOnEntry(State_t *pState, int32_t eventID)
{
	 LEDHandler_AllOff();

	DisplayHandlerSetToIdle();

	return STATETBL_ERR_OK;
}

 int32_t testModeOnEntry(State_t *pState, int32_t eventID)
{
	LEDHandler_TestMode();
	DisplayHandlerSetToIdle();

	return STATETBL_ERR_OK;
}

 int32_t operationOnEntry(State_t *pState, int32_t eventID)
{
	uint32_t actualTick = HAL_GetTick();
	context->gasSensor.lastTick = actualTick;
	context->waterSensor.lastTick = actualTick;

    context->gasSensor.elapsedWarningTime = 0;
    context->gasSensor.elapsedEmergencyTime = 0;

    context->waterSensor.elapsedWarningTime = 0;
    context->waterSensor.elapsedEmergencyTime = 0;

    context->gasSensor.warningLedTriggered = false;
    context->waterSensor.warningLedTriggered = false;
	return STATETBL_ERR_OK;
}

int32_t failureOnEntry(State_t *pState, int32_t eventID)
{
	LEDHandler_FailureMode(context->sensorDefect);

	DisplayHandlerSetToIdle();

	return STATETBL_ERR_OK;
}


/***** PRIVATE FUNCTIONS *****************************************************/

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
#include "AppGuards.h"
#include "AppContext.h"
#include "stdbool.h"
#include "stddef.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/
static ApplicationContext_t *context;

/***** PUBLIC FUNCTIONS ******************************************************/

int32_t AppGuard_Init(void)
{
	context = AppContext_Get();
	if(context == NULL)
	{
		return APP_GUARD_APP_INVALID_PTR;
	}
	return APP_GUARD_ERR_OK;
}


bool PreOpGuard(StateTableEntry_t* pEntry, int32_t eventID)
{
	if((eventID == EVT_ID_INIT_READY) || (eventID == EVT_ID_SW1_PRESSED))
		return true;

	return false;
}

bool OpGuard(StateTableEntry_t * pEntry, int32_t eventID)
{
	if((eventID == EVT_ID_SW1_PRESSED) || (eventID == EVT_ID_B1_PRESSED))
			return true;

		return false;

}

bool EmergencyGuard(StateTableEntry_t * pEntry, int32_t eventID)
{
	if((eventID == EVT_ID_TRIGGER_EMERGENCY))
			return true;

		return false;
}

bool TestModeGuard(StateTableEntry_t * pEntry, int32_t eventID)
{
	if((eventID == EVT_ID_SW2_PRESSED))
			return true;

		return false;
}

bool FailureGuard(StateTableEntry_t *pEntry, int32_t eventID)
{
	if( ((eventID == EVT_ID_ERROR) ||  (eventID == EVT_ID_STACK_CORRUPTION) || (eventID == EVT_ID_SENSOR_DEFECT)) )
	{
		if(eventID == EVT_ID_SENSOR_DEFECT)
				context->sensorDefect = true;
		return true;
	}
	return false;
}
/***** PRIVATE FUNCTIONS *****************************************************/

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

#include "AppOnStates.h"

#include "AppContext.h"
#include "DualChannelGas.h"
#include "WaterSensor.h"
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
static ApplicationContext_t* appContext = NULL;


/***** PUBLIC FUNCTIONS ******************************************************/

int32_t AppOnStates_Init()
{
	appContext = AppContext_Get();
	if(appContext == NULL)
	{
		return APP_ON_STATE_INVALID_PTR;
	}

	return APP_ON_STATE_OK;
}


int32_t onInit(State_t* pState, int32_t eventID)
{
	// check gas Sensor
	int32_t stateTableResult = STATETBL_ERR_OK;


	if(dualGasSetVoltages() != DUALSENSORS_OK)
	{
		stateTableResult = stateTableSendEvent(&appContext->stateTable, EVT_ID_ERROR);
		return stateTableResult;
	}

	int32_t dualGasSensorConsistencyResult = dualGasCheckInconsistency();
    if(dualGasSensorConsistencyResult == DUALSENSORS_OK)
    {
    	stateTableResult = stateTableSendEvent(&appContext->stateTable, EVT_ID_INIT_READY);
    	return stateTableResult;
    }

    else if(dualGasSensorConsistencyResult == DUALSENSORS_DEFECT)
    {
    	stateTableResult = stateTableSendEvent(&appContext->stateTable, EVT_ID_ERROR);
    	return stateTableResult;
    }

	return stateTableResult;
}

int32_t onPreOperational(State_t * pState, int32_t eventID)
{

	return STATETBL_ERR_OK;
}

int32_t onOperational(State_t * pState, int32_t eventID)
{
	int32_t stateTableResult = STATETBL_ERR_OK;


	LEDHandler_OperationalMode(appContext->warningMode);

	return stateTableResult;
}



int32_t onEmergency(State_t *pState, int32_t eventID)
{


	LEDHandler_EmergencyMode();

	return STATETBL_ERR_OK;
}

/***** PRIVATE FUNCTIONS *****************************************************/

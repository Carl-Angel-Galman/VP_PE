
/******************************************************************************
 * @file main.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Main file for the VP Template project
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "stm32g4xx_hal.h"
#include "System.h"

#include "HardwareConfig.h"

#include "Util/Log/printf.h"

#include "Util/Log/LogOutput.h"

#include "GlobalObjects.h"

#include "Application.h"

#include "AppTasks.h"

#include "Scheduler.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/

/***** PRIVATE VARIABLES *****************************************************/
         // Global Scheduler instance
Scheduler AppScheduler;



/***** PUBLIC FUNCTIONS ******************************************************/


/**
 * @brief Main function of System
 */
int main(void)
{
	int32_t appInitResult = AppInitialize();
	if(appInitResult == APP_INIT_ERR)
	{
		while(1);
	}

    int32_t schedulerInitialized = schedInitialize(&AppScheduler);

    AppScheduler.pTask_1ms = taskApp1ms;

    AppScheduler.pTask_10ms = taskApp10ms;

    AppScheduler.pTask_50ms = taskApp50ms;

    AppScheduler.pTask_250ms = taskApp250ms;

	 if((schedulerInitialized == SCHED_ERR_INVALID_PTR))
	{
		return APP_INIT_ERR;
	}



    while (1)
    {

    	int32_t schedulerCylcicResult = schedCycle(&AppScheduler);
    	if (schedulerCylcicResult != SCHED_ERR_OK)
    	{
    		AppSendEvent(EVT_ID_ERROR);
    	}
    }
}

/***** PRIVATE FUNCTIONS *****************************************************/



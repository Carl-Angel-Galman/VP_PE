/******************************************************************************
 * @file AppTasks.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Implementation File for the application tasks
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "Scheduler.h"

#include "AppTasks.h"

#include "Application.h"

#include "StackMonitor.h"
/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/


void taskApp1ms(void)
{
	AppDisplayDigitsOnSegments();


}



void taskApp10ms(void)
{
	int32_t ButtonPollResult =  AppPollForButtonEvent();

	if (ButtonPollResult)
	{
		AppSendEvent(ButtonPollResult);
	}


}


void taskApp50ms(void)
{
	int32_t AppRunError = AppRun();

	if(AppRunError == APP_RUN_ERR)
	{
		AppSendEvent(EVT_ID_ERROR);
	}

}

void taskApp250ms(void)
{

}


/***** PRIVATE FUNCTIONS *****************************************************/





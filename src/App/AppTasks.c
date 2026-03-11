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

#include "stdbool.h"
/***** PRIVATE CONSTANTS *****************************************************/

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

	int32_t SensorPollResult = AppUpdatingSensors();

	if(SensorPollResult != NO_EVT)
	{
		AppSendEvent(SensorPollResult);
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

#if defined(USE_CUSTOM_MSP) && (USE_CUSTOM_MSP == 1)

	extern uint8_t _sstack;

	uintptr_t stackScanner = (uintptr_t)&_sstack;
	stackScanner--;

	__disable_irq();
	__set_MSP((uint32_t)stackScanner);
	__DSB();
	__ISB();
	__enable_irq();

#endif

	uint32_t freeBytes = GetFreeBytes();
	uint32_t usedBytes = GetUsedBytes();
	uint8_t usage = GetUsage();

	(void)freeBytes;
	(void)usedBytes;
	(void)usage;

	bool stackCorrupted = isCorrupted();

	if(stackCorrupted)
	{
		AppSendEvent(EVT_ID_STACK_CORRUPTION);
	}

}


/***** PRIVATE FUNCTIONS *****************************************************/





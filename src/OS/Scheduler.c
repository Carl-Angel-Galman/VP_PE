/******************************************************************************
 * @file Scheduler.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Implementation of the cooperative scheduler with a  pre-defined set
 * of cyclic "task slots"
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "Scheduler.h"

#include "stm32g4xx_hal.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/
#define HAL_TICK_VALUE_1MS		1
#define HAL_TICK_VALUE_10MS     10      //!< Number of HAL Ticks used for 10ms Tasks
#define HAL_TICK_VALUE_50MS   	50     //!< Number of HAL Ticks used for 100ms Tasks
#define HAL_TICK_VALUE_250MS    250     //!< Number of HAL Ticks used for 250ms Tasks

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/
static uint32_t schedulerGetElapseTime(uint32_t savedTimeStamp, uint32_t currentTime);

/***** PUBLIC FUNCTIONS ******************************************************/


int32_t schedInitialize(Scheduler* pScheduler)
{
	if(pScheduler == 0)
	{
		return SCHED_ERR_INVALID_PTR;
	}

	pScheduler->halTick_10ms = 0;
	pScheduler->halTick_50ms = 0;
	pScheduler->halTick_250ms = 0;

	pScheduler->halTick_10ms = 0;
	pScheduler->halTick_50ms = 0;
	pScheduler->halTick_250ms = 0;



    return SCHED_ERR_OK;
}


int32_t schedCycle(Scheduler* pScheduler)
{
	if(pScheduler == 0)
	{
		return SCHED_ERR_INVALID_PTR;
	}


	uint32_t timeElapsed = 0;
	uint32_t actualTick= 0;

	actualTick = HAL_GetTick();
	timeElapsed = schedulerGetElapseTime(pScheduler->halTick_1ms, actualTick);
	if(timeElapsed >= HAL_TICK_VALUE_1MS)
	{
		pScheduler->halTick_1ms = actualTick;
		if(pScheduler->pTask_1ms != 0)
		{
			pScheduler->pTask_1ms();
		}else
		{
			return SCHED_ERR_INVALID_PTR;

		}
	}

	actualTick = HAL_GetTick();
	timeElapsed = schedulerGetElapseTime(pScheduler->halTick_10ms, actualTick);
	if(timeElapsed >= HAL_TICK_VALUE_50MS)
	{
		pScheduler->halTick_10ms = actualTick;
		if(pScheduler->pTask_10ms != 0)
		{
			pScheduler->pTask_10ms();
		}else
		{
			return SCHED_ERR_INVALID_PTR;

		}
	}

	actualTick = HAL_GetTick();
	timeElapsed = schedulerGetElapseTime(pScheduler->halTick_50ms, actualTick);
	if(timeElapsed >= HAL_TICK_VALUE_50MS)
	{
		pScheduler->halTick_50ms = actualTick;
		if(pScheduler->pTask_50ms != 0)
		{
			pScheduler->pTask_50ms();
		}else
		{
			return SCHED_ERR_INVALID_PTR;

		}
	}

	actualTick = HAL_GetTick();
	timeElapsed = schedulerGetElapseTime(pScheduler->halTick_250ms, actualTick);
	if(timeElapsed >= HAL_TICK_VALUE_250MS)
	{
		pScheduler->halTick_250ms = actualTick;
		if(pScheduler->halTick_250ms != 0)
		{
			pScheduler->pTask_250ms();
		}else
		{
			return SCHED_ERR_INVALID_PTR;

		}
	}


	return SCHED_ERR_OK;
}


/***** PRIVATE FUNCTIONS *****************************************************/

static inline uint32_t schedulerGetElapseTime(uint32_t savedTimeStamp, uint32_t currentTime)
{
	uint32_t dt = currentTime - savedTimeStamp;
	return dt;
}


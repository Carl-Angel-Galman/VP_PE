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
 * @brief Implementation of a cooperative scheduler with predefined cyclic task
 * slots.
 *
 * @details
 * This module implements a simple cooperative scheduler based on the HAL system
 * tick. It supports cyclic execution of task functions in fixed time slots:
 * 1 ms, 10 ms, 50 ms, and 250 ms.
 *
 * The scheduler compares the current HAL tick value with the stored timestamps
 * of the individual task slots. If the configured period has elapsed, the
 * corresponding task function is executed.
 *
 * @note
 * The scheduler is cooperative. Tasks are executed sequentially and must not
 * block for a long time.
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "Scheduler.h"

#include "stm32g4xx_hal.h"

#include <stddef.h>

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/

/** @brief Number of HAL ticks corresponding to 1 ms. */
#define HAL_TICK_VALUE_1MS      1U

/** @brief Number of HAL ticks corresponding to 10 ms. */
#define HAL_TICK_VALUE_10MS     10U

/** @brief Number of HAL ticks corresponding to 50 ms. */
#define HAL_TICK_VALUE_50MS     50U

/** @brief Number of HAL ticks corresponding to 250 ms. */
#define HAL_TICK_VALUE_250MS    250U

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/

/**
 * @brief Calculates the elapsed time between two HAL tick values.
 *
 * @details
 * The calculation is based on unsigned subtraction and therefore also works
 * correctly when the HAL tick counter wraps around.
 *
 * @param[in] savedTimeStamp
 * Previously stored tick value.
 *
 * @param[in] currentTime
 * Current HAL tick value.
 *
 * @return Elapsed time in ticks.
 */
static inline uint32_t schedulerGetElapseTime(uint32_t savedTimeStamp, uint32_t currentTime);


static int32_t executeCyclicTask(CyclicFunction task, uint32_t* tick ,uint32_t threshold);
/***** PUBLIC FUNCTIONS ******************************************************/

/**
 * @brief Initializes the scheduler instance.
 *
 * @details
 * Resets all internal timestamp values of the scheduler.
 *
 * @param[in,out] pScheduler
 * Pointer to the scheduler instance to initialize.
 *
 * @retval SCHED_ERR_OK
 * Scheduler was initialized successfully.
 *
 * @retval SCHED_ERR_INVALID_PTR
 * `pScheduler` is `NULL`.
 */
int32_t schedInitialize(Scheduler* pScheduler)
{
	if(pScheduler == 0)
	{
		return SCHED_ERR_INVALID_PTR;
	}

	pScheduler->halTick_1ms = 0;

	pScheduler->halTick_10ms = 0;
	pScheduler->halTick_50ms = 0;
	pScheduler->halTick_250ms = 0;

	pScheduler->pTask_1ms = 0;
	pScheduler->pTask_10ms = 0;
	pScheduler->pTask_50ms = 0;
	pScheduler->pTask_250ms = 0;



    return SCHED_ERR_OK;
}


/**
 * @brief Executes one scheduler cycle.
 *
 * @details
 * Checks all configured scheduler time slots and executes the corresponding
 * task function if the required period has elapsed since the last execution.
 *
 * The following cyclic tasks are supported:
 * - 1 ms task
 * - 10 ms task
 * - 50 ms task
 * - 250 ms task
 *
 * @param[in,out] pScheduler
 * Pointer to the scheduler instance.
 *
 * @retval SCHED_ERR_OK
 * Scheduler cycle completed successfully.
 *
 * @retval SCHED_ERR_INVALID_PTR
 * `pScheduler` is `NULL` or a required task pointer is `NULL`.
 */
int32_t schedCycle(Scheduler* pScheduler)
{
	if(pScheduler == 0)
	{
		return SCHED_ERR_INVALID_PTR;
	}

	uint32_t executionResult= 0u;

	executionResult = executeCyclicTask(pScheduler->pTask_1ms, &pScheduler->halTick_1ms ,HAL_TICK_VALUE_1MS );

	if(executionResult == SCHED_ERR_INVALID_PTR)
	{
		//Do nothing
	}
	executionResult = executeCyclicTask(pScheduler->pTask_10ms, &pScheduler->halTick_10ms ,HAL_TICK_VALUE_10MS );

	if(executionResult == SCHED_ERR_INVALID_PTR)
		{
			//Do nothing
		}
	executionResult = executeCyclicTask(pScheduler->pTask_50ms, &pScheduler->halTick_50ms ,HAL_TICK_VALUE_50MS );

	if(executionResult == SCHED_ERR_INVALID_PTR)
		{
			//Do nothing
		}
	executionResult = executeCyclicTask(pScheduler->pTask_250ms, &pScheduler->halTick_250ms ,HAL_TICK_VALUE_250MS );

	if(executionResult == SCHED_ERR_INVALID_PTR)
		{
			//Do nothing
		}


	return SCHED_ERR_OK;
}


/***** PRIVATE FUNCTIONS *****************************************************/

/**
 * @brief Calculates elapsed scheduler time in HAL ticks.
 *
 * @param[in] savedTimeStamp
 * Previously stored timestamp.
 *
 * @param[in] currentTime
 * Current HAL tick value.
 *
 * @return Difference between current and saved timestamp in ticks.
 */
static inline uint32_t schedulerGetElapseTime(uint32_t savedTimeStamp, uint32_t currentTime)
{
	uint32_t dt = currentTime - savedTimeStamp;
	return dt;
}

static int32_t executeCyclicTask(CyclicFunction task, uint32_t* tick ,uint32_t threshold)
{
	if(tick == NULL || threshold < 0)
	{
		return SCHED_ERR_INVALID_PTR;
	}

	uint32_t actualTick = HAL_GetTick();
	uint32_t timeElapsed = schedulerGetElapseTime(*tick, actualTick);
		if(timeElapsed >= threshold)
		{
			*tick = actualTick;
			if(task != 0)
			{
				task();
			}else
			{
				return SCHED_ERR_INVALID_PTR;

			}
		}

	return SCHED_ERR_OK;
}


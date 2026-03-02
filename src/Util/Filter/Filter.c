/******************************************************************************
 * @file Filter.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Implementation file for Filter library
 *
 *
 *****************************************************************************/

 /***** INCLUDES **************************************************************/
#include "Filter.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/

int32_t filterInitEMA(EMAFilterData_t* pEMA, int32_t scalingFactor, int32_t alpha, bool resetFilter)
{
	if(pEMA == NULL)
		{
			return FILTER_ERR_INVALID_PTR;
		}
	if(scalingFactor = 0)
	{
		return FILTER_ERR_INVALID_PARAM;
	}
	pEMA->scalingFactor = scalingFactor;
	pEMA->alpha = alpha;
	pEMA->firstValueAvailable  = false;

    return FILTER_ERR_OK;
}

int32_t filterResetEMA(EMAFilterData_t* pEMA)
{
    return 0;
}

int32_t filterEMA(EMAFilterData_t* pEMA, int32_t sensorValue)
{
	if(pEMA == NULL)
	{
		return FILTER_ERR_INVALID_PTR;
	}
	//First Sensorvalue
	if(pEMA->firstValueAvailable == false)
	{
		pEMA->previousValue = sensorValue;
		pEMA->firstValueAvailable = true;
		return sensorValue;
	}
	//Calculate new Valute with EMA Filterin
	int32_t newValue = (int32_t)(pEMA->alpha * sensorValue + (pEMA->scalingFactor-pEMA->alpha)* pEMA->previousValue)/pEMA->scalingFactor;
    return newValue;
}

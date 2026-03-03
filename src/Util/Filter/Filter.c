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

int32_t filterInitEMA(EMAFilterData_t* pEMA, int32_t scalingFactor, int32_t alpha)
{
	if(pEMA == NULL)
		{
			return FILTER_ERR_INVALID_PTR;
		}
<<<<<<< HEAD
	if(scalingFactor = 0)
=======
	//Check if scalingFactor is 0 or negative
	if(scalingFactor <= CHECK_NULL_NEG)
	{
		return FILTER_ERR_INVALID_PARAM;
	}
	//Check if alpha is between 0 and  1
	if(alpha <= CHECK_NULL_NEG || alpha > scalingFactor)
>>>>>>> 5a5441275a3a915ce293841d1a97457689a6c849
	{
		return FILTER_ERR_INVALID_PARAM;
	}
	pEMA->scalingFactor = scalingFactor;
	pEMA->alpha = alpha;
<<<<<<< HEAD
	pEMA->firstValueAvailable  = false;
=======
	pEMA->firstValueAvailable = false;
>>>>>>> 5a5441275a3a915ce293841d1a97457689a6c849

    return FILTER_ERR_OK;
}

int32_t filterResetEMA(EMAFilterData_t* pEMA)
{
    if(pEMA == NULL)
	{
		return FILTER_ERR_INVALID_PTR;
	}
	//Reset filter and overwrite previous Value
	pEMA->firstValueAvailable = false;
	pEMA->previousValue = 0;

    return FILTER_ERR_OK;
}

int32_t filterEMA(EMAFilterData_t* pEMA, int32_t sensorValue)
{
	if(pEMA == NULL)
	{
		return FILTER_ERR_INVALID_PTR;
	}
<<<<<<< HEAD
	//First Sensorvalue
=======

    // First sample initializes filter output
>>>>>>> 5a5441275a3a915ce293841d1a97457689a6c849
	if(pEMA->firstValueAvailable == false)
	{
		pEMA->previousValue = sensorValue;
		pEMA->firstValueAvailable = true;
		return sensorValue;
	}
<<<<<<< HEAD
	//Calculate new Valute with EMA Filterin
	int32_t newValue = (int32_t)(pEMA->alpha * sensorValue + (pEMA->scalingFactor-pEMA->alpha)* pEMA->previousValue)/pEMA->scalingFactor;
=======

	//Calculate new Value with EMA Filtering
	//EMA Implementation using scaled integer arithmetic: y[n]= alpha*x[n] + (1-alpha)*y[n-1]
	int32_t newValue = (int32_t)(pEMA->alpha * sensorValue + (pEMA->scalingFactor-pEMA->alpha)* pEMA->previousValue)/pEMA->scalingFactor;
	pEMA->previousValue = newValue;
>>>>>>> 5a5441275a3a915ce293841d1a97457689a6c849
    return newValue;
}

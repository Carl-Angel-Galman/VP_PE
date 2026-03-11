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
#include "DisplayHandler.h"

#include "DisplayModule.h"


#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_rcc.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/

static int8_t leftDigit = DIGIT_DASH;

static int8_t rightDigit = DIGIT_DASH;

/***** PUBLIC FUNCTIONS ******************************************************/


/***** PRIVATE FUNCTIONS *****************************************************/

int32_t DisplayHandlerSetToIdle(void)
{
	leftDigit = DIGIT_DASH;

	rightDigit = DIGIT_DASH;

	return DH_ERR_OK;
}





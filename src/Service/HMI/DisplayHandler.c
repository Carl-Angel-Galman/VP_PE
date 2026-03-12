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



#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_rcc.h"

#include "DisplayModule.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/

#define HUNDREDS_DIGIT 100

#define TENTH_DIGIT 10

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/
static inline Display_t  alternateDisplays(void);

/***** PRIVATE VARIABLES *****************************************************/

static int8_t leftDigit = 0;

static int8_t rightDigit = 0;

static Display_t currentDisplay = LEFT_DISPLAY;

/***** PUBLIC FUNCTIONS ******************************************************/


/***** PRIVATE FUNCTIONS *****************************************************/

int32_t DisplayHandlerInit(void)
{

    displayInitialize();

	leftDigit = DIGIT_DASH;
	rightDigit = DIGIT_DASH;
	return DH_ERR_OK;
}

int32_t DisplayHandlerSetToIdle(void)
{
	leftDigit = DIGIT_DASH;
		rightDigit = DIGIT_DASH;
	return DH_ERR_OK;
}

int32_t DisplayHandlerSetDigits(uint32_t currentWaterLevel)
{

	leftDigit  = currentWaterLevel / HUNDREDS_DIGIT;
	rightDigit = (currentWaterLevel / TENTH_DIGIT) % TENTH_DIGIT;


	return DISPLAY_ERR_OK;
}

int32_t DisplayHandlerDisplayTwoDigits(void)
{
	currentDisplay = alternateDisplays();

	displayShowDigit(currentDisplay, (currentDisplay ?  rightDigit: leftDigit));

	return DISPLAY_ERR_OK;
}

/***** PRIVATE FUNCTIONS *****************************************************/
static inline Display_t  alternateDisplays(void)
{
	return currentDisplay ^ 1 ;
}






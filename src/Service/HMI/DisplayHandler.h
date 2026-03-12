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
#ifndef _DISPLAY_HANDLER_H_
#define _DISPLAY_HANDLER_H_




/***** INCLUDES **************************************************************/

#include "stm32g4xx.h"

#include "stm32g4xx_hal.h"

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/
#define DH_ERR_OK 0


/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/

int32_t DisplayHandlerInit(void);

int32_t DisplayHandlerSetToIdle(void);

int32_t DisplayHandlerSetDigits(uint32_t waterLevel);

int32_t DisplayHandlerDisplayTwoDigits(void);
#endif

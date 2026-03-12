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
#ifndef _BUTTONHANDLER_H_
#define _BUTTONHANDLER_H_


/***** INCLUDES **************************************************************/
#include "ButtonModule.h"
#include <stdbool.h>
#include <stdint.h>

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/

#define BH_ERR_OK 0
/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/
bool ButtonHandlerhasDebounced(Button_t button);

int32_t ButtonHandlerInit(void);

#endif

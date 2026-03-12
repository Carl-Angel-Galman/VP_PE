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
#ifndef _APP_ON_STATE_H
#define _APP_ON_STATE_H


/***** INCLUDES **************************************************************/

#include "stm32g4xx.h"
#include  <stdint.h>
#include "StateTable/StateTable.h"

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/
#define APP_ON_STATE_INVALID_PTR -1

#define APP_ON_STATE_OK 0

/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/

int32_t AppOnStates_Init();

int32_t onInit(State_t* pState, int32_t eventID);

int32_t onPreOperational(State_t* pState, int32_t eventID);

int32_t onOperational(State_t* pState, int32_t eventID);

int32_t onEmergency(State_t* pState, int32_t eventID);



#endif

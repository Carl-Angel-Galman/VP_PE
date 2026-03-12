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
#ifndef _ON_ENTRY_H
#define _ON_ENTRY_H


/***** INCLUDES **************************************************************/
#include "stdint.h"
#include "StateTable/StateTable.h"

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/
#define APP_ON_ENTRY_OK 0
#define APP_ON_ENTRY_ERR_INVALID_PTR -1

/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/
int32_t AppOnEntry_Init(void);

int32_t initOnEntry(State_t* pState, int32_t eventID);

int32_t displayDashOnEntry(State_t *pState, int32_t eventID);

int32_t failureOnEntry(State_t *pState, int32_t eventID);

int32_t preOperationOnEntry(State_t *pState, int32_t eventID);

int32_t operationOnEntry(State_t *pState, int32_t eventID);

int32_t testModeOnEntry(State_t *pState, int32_t eventID);

#endif

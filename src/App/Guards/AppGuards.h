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
#ifndef _GUARDS_H
#define _GUARDS_H


/***** INCLUDES **************************************************************/

#include "StateTable/StateTable.h"
#include <stdint.h>
#include <stdbool.h>

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/
#define APP_GUARD_APP_INVALID_PTR -1

#define APP_GUARD_ERR_OK 0

/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/
int32_t AppGuard_Init(void);

 bool PreOpGuard(StateTableEntry_t* pEntry, int32_t eventID);

 bool OpGuard(StateTableEntry_t * pEntry, int32_t eventID);

 bool EmergencyGuard(StateTableEntry_t * pEntry, int32_t eventID);

 bool FailureGuard(StateTableEntry_t * pEntry, int32_t eventID);

 bool TestModeGuard(StateTableEntry_t * pEntry, int32_t eventID);

#endif

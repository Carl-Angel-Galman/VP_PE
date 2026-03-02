/******************************************************************************
 * @file Application.h
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Header file for main application (state machine)
 *
 *
 *****************************************************************************/
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

/***** INCLUDES **************************************************************/
#include <stdint.h>

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/
#define STATE_ID_INIT        			1UL       //!< Example State for Startup
#define STATE_ID_OPERATIONAL        	2UL       //!< Example State for Runing
#define STATE_ID_FAILURE        		3UL       //!< Example State for Failure
#define STATE_ID_PREOPERATIONAL 		4UL
#define STATE_ID_TESTMODE 				5UL
#define STATE_ID_EMERGENCY				6UL

#define APP_NO_ERR 				0
#define APP_INIT_ERR			-1
#define APP_RUN_ERR 			-2

/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/

int32_t AppInitialize(void);

int32_t AppRun(void);

int32_t AppSendEvent(int32_t eventID);

int32_t AppPollForButtonEvent(void);

int32_t AppDisplayDigitsOnSegments(void);


#endif

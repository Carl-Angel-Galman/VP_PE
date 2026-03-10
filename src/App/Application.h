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
#include <stdbool.h>

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/
#define STATE_ID_INIT        			1UL       //!< Example State for Startup
#define STATE_ID_OPERATIONAL        	2UL       //!< Example State for Runing
#define STATE_ID_FAILURE        		3UL       //!< Example State for Failure
#define STATE_ID_PREOPERATIONAL 		4UL
#define STATE_ID_TESTMODE 				5UL
#define STATE_ID_EMERGENCY				6UL

#define APP_NO_ERR 						0
#define APP_INIT_ERR					-1
#define APP_RUN_ERR 					-2

#define NO_EVT							0
#define EVT_ID_INIT_READY       		1       //!< Event ID for INIT_READY
#define EVT_ID_ERROR    				2       //!< Event ID for Sensor Failure
#define EVT_ID_SW1_PRESSED				3
#define EVT_ID_SW2_PRESSED				4
#define EVT_ID_B1_PRESSED				5
#define EVT_ID_TO_TESTMODE 				6
#define EVT_ID_STACK_CORRUPTION			7
#define EVT_ID_TRIGGER_EMERGENCY 		8
#define EVT_ID_SENSOR_DEFECT			9


typedef struct
{
    int32_t warningThreshold;
    int32_t emergencyThreshold;

    uint32_t elapsedWarningTime;
    uint32_t elapsedEmergencyTime;
    uint32_t lastTick;

    uint32_t warningTime;
    uint32_t emergencyTime;

    bool warningLedTriggered;

} SensorMonitor_t;
/***** TYPES *****************************************************************/


/***** TYPES *****************************************************************/

/***** PROTOTYPES ************************************************************/

int32_t AppInitialize(void);

int32_t AppRun(void);

int32_t AppSendEvent(int32_t eventID);

int32_t AppPollForButtonEvent(void);

int32_t AppDisplayDigitsOnSegments(void);


#endif

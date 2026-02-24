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
#define STATE_ID_INIT        1       //!< Example State for Startup
#define STATE_ID_OPERATIONAL        2       //!< Example State for Runing
#define STATE_ID_FAILURE        3       //!< Example State for Failure
#define STATE_ID_PREOPERATIONAL 4
#define STATE_ID_TESTMODE 		5
#define STATE_ID_EMERGENCY		6

#define EVT_ID_INIT_READY       1       //!< Event ID for INIT_READY
#define EVT_ID_SENSOR_FAILED    2       //!< Event ID for Sensor Failure
#define EVT_ID_SWITCH_OPERATIONAL 3
#define EVT_ID_SWITCH_PREOPERATIONAL 4
#define EVT_ID_SENSOR_DEFECT 5
#define EVT_ID_TO_TESTMODE 6
#define EVT_ID_ALARM_RESET 7
#define EVT_ID_TRIGGER_EMERGENCY 8

/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/

int32_t sampleAppInitialize();

int32_t sampleAppRun();

int32_t sameplAppSendEvent(int32_t eventID);

#endif

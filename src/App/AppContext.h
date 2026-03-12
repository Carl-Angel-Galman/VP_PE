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
#ifndef _CONTEXT_H
#define _CONTEXT_H


/***** INCLUDES **************************************************************/
#include <stdbool.h>

#include "StateTable/StateTable.h"
#include <stdint.h>


/***** CONSTANTS *************************************************************/
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


/***** MACROS ****************************************************************/


/***** TYPES *****************************************************************/

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

typedef struct
{
    bool warningMode;
    bool sensorDefect;
    SensorMonitor_t gasSensor;
    SensorMonitor_t waterSensor;
    StateTable_t stateTable;
} ApplicationContext_t;


/***** PROTOTYPES ************************************************************/



void AppContext_Set(ApplicationContext_t * ctx);

ApplicationContext_t* AppContext_Get(void);

#endif

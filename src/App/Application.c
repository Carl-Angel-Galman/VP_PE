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
 * @brief Implementation file for main application (state machine)
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include <string.h>

#include "Application.h"


#include "Util/Log/printf.h"


#include "UARTModule.h"

#include "ButtonModule.h"

#include "HMI/LEDHandler.h"

#include "DisplayModule.h"

#include "DualChannelGas.h"

#include "WaterSensor.h"

#include "Util/StateTable/StateTable.h"

#include "ADCModule.h"

#include "TimerModule.h"

#include "System.h"

#include "stm32g4xx_hal.h"




/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/
#define GAS_SENSOR_WARNING_THRESHHOLD 3000

#define GAS_SENSOR_EMERGENCY_THRESHHOLD 5000

#define TEN_SEC_THRESHOLD_50MS 10000

#define FIVE_SEC_THRESHOLD_50MS 5000

#define THREE_SEC_THRESHOLD_50MS 3000

#define COUNTER_HAS_REACHED_TEN_SECS_50MS(counter) (counter >= TEN_SEC_THRESHOLD_50MS)

#define COUNTER_HAS_REACHED_FIVE_SECS_50MS(counter) (counter >= FIVE_SEC_THRESHOLD_50MS)

#define COUNTER_HAS_REACHED_THREE_SECS_50MS(counter) (counter >= THREE_SEC_THRESHOLD_50MS)


#define WATER_SENSOR_WARNING_THRESHHOLD 250 //in cm

#define WATER_SENSOR_EMERGENCY_THRESHHOLD 300

#define MAX_DISPLAY_NUMBER 999

#define HUNDREDS_DIGIT 100

#define TENS_DIGIT 10

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/

static int32_t initOnEntry(State_t* pState, int32_t eventID);

static int32_t onInit(State_t* pState, int32_t eventID);

static int32_t onPreOperational(State_t* pState, int32_t eventID);

static int32_t onOperational(State_t* pState, int32_t eventID);

static int32_t onEmergency(State_t* pState, int32_t eventID);

static int32_t displayDashOnEntry(State_t *pState, int32_t eventID);

static int32_t failureOnEntry(State_t *pState, int32_t eventID);

static int32_t preOperationOnEntry(State_t *pState, int32_t eventID);

static int32_t operationOnEntry(State_t *pState, int32_t eventID);

static int32_t testModeOnEntry(State_t *pState, int32_t eventID);



static bool PreOpGuard(StateTableEntry_t* pEntry, int32_t eventID);

static bool OpGuard(StateTableEntry_t * pEntry, int32_t eventID);

static bool EmergencyGuard(StateTableEntry_t * pEntry, int32_t eventID);

static bool FailureGuard(StateTableEntry_t * pEntry, int32_t eventID);

static bool TestModeGuard(StateTableEntry_t * pEntry, int32_t eventID);

static void changeVectorTable(void);

static int32_t initializePeripherals(void);

static int32_t monitorSensor(int32_t value, SensorMonitor_t *sensor);



/***** PRIVATE VARIABLES *****************************************************/

static SensorMonitor_t gasSensor =
{
    .warningThreshold = GAS_SENSOR_WARNING_THRESHHOLD,
    .emergencyThreshold = GAS_SENSOR_EMERGENCY_THRESHHOLD,
    .warningTime = FIVE_SEC_THRESHOLD_50MS,
    .emergencyTime = THREE_SEC_THRESHOLD_50MS
};

static SensorMonitor_t waterSensor =
{
    .warningThreshold = WATER_SENSOR_WARNING_THRESHHOLD,
    .emergencyThreshold = WATER_SENSOR_EMERGENCY_THRESHHOLD,
    .warningTime = TEN_SEC_THRESHOLD_50MS,
    .emergencyTime = FIVE_SEC_THRESHOLD_50MS
};


static State_t gStateList[] =
{
		{STATE_ID_INIT, 			displayDashOnEntry,  				onInit,                  	0,              false},

		{STATE_ID_PREOPERATIONAL, 	preOperationOnEntry,  				onPreOperational,           0,              false},

		{STATE_ID_OPERATIONAL, 		operationOnEntry,       			onOperational, 	    		0,				false},

		{STATE_ID_FAILURE, 			failureOnEntry,  					0,                 			0,              false},

		{STATE_ID_TESTMODE, 		testModeOnEntry,  				    0,                  	 	0,				false},

		{STATE_ID_EMERGENCY, 		displayDashOnEntry,  				onEmergency,                0,              false}
};

/**
 * @brief Definition of the transistion table of the state machine. Each row
 * contains FROM_STATE_ID, TO_STATE_ID, EVENT_ID, Function Pointer Guard Function
 *
 * The last two members of a transistion row are only the initialization of dynamic
 * members used durin runtim
 */
static StateTableEntry_t gStateTableEntries[] =
{
	{STATE_ID_INIT, 				STATE_ID_PREOPERATIONAL,          	EVT_ID_INIT_READY,          	PreOpGuard,      	&gStateList[0],      	&gStateList[1]},

	{STATE_ID_INIT,          		STATE_ID_FAILURE,           		EVT_ID_ERROR,       	        FailureGuard,      	&gStateList[0],      	&gStateList[3]},

	{STATE_ID_PREOPERATIONAL, 		STATE_ID_OPERATIONAL,           	EVT_ID_SW1_PRESSED,      		OpGuard,      		&gStateList[1],      	&gStateList[2]},

	{STATE_ID_OPERATIONAL, 			STATE_ID_PREOPERATIONAL, 			EVT_ID_SW1_PRESSED, 			PreOpGuard,			&gStateList[2],      	&gStateList[1]},

	{STATE_ID_OPERATIONAL, 			STATE_ID_FAILURE, 					EVT_ID_ERROR, 					FailureGuard,		&gStateList[2],      	&gStateList[3]},

	{STATE_ID_OPERATIONAL, 			STATE_ID_EMERGENCY, 				EVT_ID_TRIGGER_EMERGENCY, 		EmergencyGuard,		&gStateList[2],      	&gStateList[5]},

	{STATE_ID_OPERATIONAL, 			STATE_ID_FAILURE, 					EVT_ID_SENSOR_DEFECT, 			FailureGuard,		&gStateList[2],      	&gStateList[3]},

	{STATE_ID_OPERATIONAL, 			STATE_ID_TESTMODE, 					EVT_ID_SW2_PRESSED, 			TestModeGuard,		&gStateList[2],      	&gStateList[5]},

	{STATE_ID_EMERGENCY, 			STATE_ID_OPERATIONAL, 				EVT_ID_B1_PRESSED, 				OpGuard,			&gStateList[5],      	&gStateList[2]},

	{STATE_ID_TESTMODE, 			STATE_ID_FAILURE,           		EVT_ID_STACK_CORRUPTION,      	FailureGuard,      	&gStateList[4],      	&gStateList[3]},

	{STATE_ID_EMERGENCY, 			STATE_ID_FAILURE, 					EVT_ID_STACK_CORRUPTION, 		FailureGuard,		&gStateList[5],      	&gStateList[3]},

	{STATE_ID_PREOPERATIONAL, 		STATE_ID_FAILURE,           		EVT_ID_STACK_CORRUPTION,      	FailureGuard,      	&gStateList[1],      	&gStateList[3]},

	{STATE_ID_OPERATIONAL, 			STATE_ID_FAILURE, 					EVT_ID_STACK_CORRUPTION, 		FailureGuard,		&gStateList[2],      	&gStateList[3]},

	{STATE_ID_INIT,          		STATE_ID_FAILURE,           		EVT_ID_STACK_CORRUPTION,       	FailureGuard,      	&gStateList[0],      	&gStateList[3]}
};


/**
 * @brief Global State Table instance
 *
 */
static StateTable_t gStateTable;

static int8_t leftDigit = DIGIT_DASH;

static int8_t rightDigit = DIGIT_DASH;

static bool warningMode = false;
static bool sensorDefect = false;



/***** PUBLIC FUNCTIONS ******************************************************/

int32_t AppInitialize(void)
{
	changeVectorTable();

	HAL_Init();

	SystemClock_Config();

	initializePeripherals();

	//Initialize gasSensor Modul and check if it is ok
	int32_t dualGasInitRes = dualGasInit();
	if(dualGasInitRes != DUALSENSORS_OK)
		AppSendEvent(EVT_ID_ERROR);

	//Initialize WaterSensor Modul and check if it is ok
	int32_t waterInitRes = waterSensorInitalize();
	if(waterInitRes != WATER_SENSOR_OK)
			AppSendEvent(EVT_ID_ERROR);

    gStateTable.pStateList = gStateList;

    gStateTable.stateCount = sizeof(gStateList) / sizeof(State_t);

    int32_t tableInitialized = stateTableInitialize(&gStateTable,

    		gStateTableEntries,

			sizeof(gStateTableEntries) / sizeof(StateTableEntry_t),

			STATE_ID_INIT);

    if((tableInitialized == STATETBL_ERR_INVALID_PTR))
	{
		return APP_INIT_ERR;
	}

    return APP_NO_ERR;
}

int32_t AppRun(void)
{
    int32_t StateTableResult = stateTableRunCyclic(&gStateTable);


    if(StateTableResult == STATETBL_ERR_INVALID_PTR)
    {
    	return APP_RUN_ERR;
    }

    return APP_NO_ERR;
}

int32_t AppSendEvent(int32_t eventID)
{
    int32_t result = stateTableSendEvent(&gStateTable, eventID);
    return result;
}


//priority 1-> B1 because reset, 2-> SW2, 3-> SW1
int32_t AppPollForButtonEvent(void)
{

	if(buttonhasButtonDebounced(BTN_SW1))
	{
		return EVT_ID_SW1_PRESSED;
	}
	if(buttonhasButtonDebounced(BTN_SW2))
	{
		return EVT_ID_SW2_PRESSED;
	}
	if(buttonhasButtonDebounced(BTN_B1))
	{
		return EVT_ID_B1_PRESSED;
	}
	return NO_EVT;
}

int32_t AppDisplayDigitsOnSegments(void)
{
	displayTwoDigits(leftDigit, rightDigit);
	return APP_NO_ERR;
}

int32_t AppUpdatingSensors()
{
	if(gStateTable.currentStateID == STATE_ID_OPERATIONAL )
	{
			int32_t currentAverage = 0;
			int32_t currentWaterLevel = 0;

			int32_t event;


			if(dualGasSetVoltages() != DUALSENSORS_OK)
				{
				    return EVT_ID_ERROR;
				}

			if(dualGasCheckInconsistency() == DUALSENSORS_DEFECT)
				{
				    return EVT_ID_SENSOR_DEFECT;
				}

			if(dualGasGetAverage(&currentAverage) != DUALSENSORS_OK)
				{
				    return EVT_ID_ERROR;
				}

			event = monitorSensor(currentAverage, &gasSensor);
			if(event != NO_EVT)
				{
				    return event;
				}

			//Warning, Emergency and Failure Logic for the watersensor
			if(waterSensorSetSensorVoltage() != WATER_SENSOR_OK)
				{
					return EVT_ID_ERROR;
				}

			if(waterSensorGetSensorValue(&currentWaterLevel) != WATER_SENSOR_OK)
				{
					return EVT_ID_ERROR;
				}

			if(currentWaterLevel > MAX_DISPLAY_NUMBER)
				   currentWaterLevel = MAX_DISPLAY_NUMBER;

			leftDigit  = currentWaterLevel / HUNDREDS_DIGIT;
			rightDigit = (currentWaterLevel / TENS_DIGIT) % TENS_DIGIT;

			event = monitorSensor(currentWaterLevel, &waterSensor);
			if(event != NO_EVT)
			{
				return event;
			}

			if(gasSensor.warningLedTriggered == false && waterSensor.warningLedTriggered == false)
			{
				ledSetLED(LED1, LED_OFF);
			}
	}

	return NO_EVT;
}


/***** PRIVATE FUNCTIONS *****************************************************/


static void changeVectorTable(void)
{
    const uint32_t app_base = 0x08010200;

    __disable_irq();

    //HAL_RCC_DeInit();
    HAL_DeInit();

    SCB->VTOR = app_base;

    __DSB(); __ISB();

    __HAL_RCC_AHB1_FORCE_RESET();
    __HAL_RCC_AHB1_RELEASE_RESET();
    __enable_irq();
    // Initialize the HAL
}


/**
 * @brief Initializes the used peripherals like GPIO,
 * ADC, DMA and Timer Interrupts
 *
 * @return Returns ERROR_OK if no error occurred
 */
static int32_t initializePeripherals(void)
{
    // Initialize UART used for Debug-Outputs
    uartInitialize(115200);

    // Initialize GPIOs for LED and 7-Segment output
    ledInitialize();

    displayInitialize();

    // Initialize GPIOs for Buttons
    buttonInitialize();
    // Initialize Timer, DMA and ADC for sensor measurements
    timerInitialize();

    adcInitialize();

    return APP_NO_ERR;

}

static int32_t monitorSensor(int32_t value, SensorMonitor_t *sensor)
{
	uint32_t actualTick = HAL_GetTick();
	 uint32_t timeElapsed = actualTick - sensor->lastTick;
	    sensor->lastTick = actualTick;

	 /* Warning Timing */
    if(value > sensor->warningThreshold)
    {
    	sensor->elapsedWarningTime += timeElapsed;
    }
    else
    {
    	sensor->elapsedWarningTime = 0;
    	sensor->warningLedTriggered = false;
    }

    if(value > sensor->emergencyThreshold)
    {
    	sensor->elapsedEmergencyTime += timeElapsed;
    }
    else
    {
        sensor->elapsedEmergencyTime = 0;
    }

    if(!sensor->warningLedTriggered &&
       sensor->elapsedWarningTime >= sensor->warningTime)
    {
        sensor->warningLedTriggered = true;
        warningMode =true;
    }

    if(sensor->elapsedEmergencyTime >= sensor->emergencyTime)
    {
        sensor->elapsedEmergencyTime = 0;
        return EVT_ID_TRIGGER_EMERGENCY;
    }

    return NO_EVT;
}

static int32_t initOnEntry(State_t* pState, int32_t eventID)
{

	LEDHandler_AllOff();

	return STATETBL_ERR_OK;
}

static int32_t onInit(State_t* pState, int32_t eventID)
{
	// check gas Sensor
	int32_t stateTableResult = STATETBL_ERR_OK;


	if(dualGasSetVoltages() != DUALSENSORS_OK)
	{
		stateTableResult = stateTableSendEvent(&gStateTable, EVT_ID_ERROR);
		return stateTableResult;
	}

	int32_t dualGasSensorConsistencyResult = dualGasCheckInconsistency();
    if(dualGasSensorConsistencyResult == DUALSENSORS_OK)
    {
    	stateTableResult = stateTableSendEvent(&gStateTable, EVT_ID_INIT_READY);
    	return stateTableResult;
    }

    else if(dualGasSensorConsistencyResult == DUALSENSORS_DEFECT)
    {
    	stateTableResult = stateTableSendEvent(&gStateTable, EVT_ID_ERROR);
    	return stateTableResult;
    }


	return stateTableResult;
}

static int32_t onPreOperational(State_t * pState, int32_t eventID)
{

	return STATETBL_ERR_OK;
}

static int32_t onOperational(State_t * pState, int32_t eventID)
{
	int32_t stateTableResult = STATETBL_ERR_OK;

	LEDHandler_OperationalMode(warningMode);

	return stateTableResult;
}



static int32_t onEmergency(State_t *pState, int32_t eventID)
{
	LEDHandler_EmergencyMode();

	return STATETBL_ERR_OK;
}

static int32_t displayDashOnEntry(State_t *pState, int32_t eventID)
{

	leftDigit = DIGIT_DASH;

	rightDigit = DIGIT_DASH;
	return STATETBL_ERR_OK;
}

static int32_t preOperationOnEntry(State_t *pState, int32_t eventID)
{
	leftDigit = DIGIT_DASH;
		rightDigit = DIGIT_DASH;
	return STATETBL_ERR_OK;
}

static int32_t testModeOnEntry(State_t *pState, int32_t eventID)
{
	LEDHandler_TestMode();
	leftDigit = DIGIT_DASH;
	rightDigit = DIGIT_DASH;
	return STATETBL_ERR_OK;
}

static int32_t operationOnEntry(State_t *pState, int32_t eventID)
{
	uint32_t actualTick = HAL_GetTick();
	gasSensor.lastTick = actualTick;
	waterSensor.lastTick = actualTick;

    gasSensor.elapsedWarningTime = 0;
    gasSensor.elapsedEmergencyTime = 0;

    waterSensor.elapsedWarningTime = 0;
    waterSensor.elapsedEmergencyTime = 0;

    gasSensor.warningLedTriggered = false;
    waterSensor.warningLedTriggered = false;
	return STATETBL_ERR_OK;
}

static int32_t failureOnEntry(State_t *pState, int32_t eventID)
{
	LEDHandler_FailureMode(sensorDefect);
	leftDigit = DIGIT_DASH;
	rightDigit = DIGIT_DASH;
	return STATETBL_ERR_OK;
}


static bool PreOpGuard(StateTableEntry_t* pEntry, int32_t eventID)
{
	if((eventID == EVT_ID_INIT_READY) || (eventID == EVT_ID_SW1_PRESSED))
		return true;

	return false;
}

static bool OpGuard(StateTableEntry_t * pEntry, int32_t eventID)
{
	if((eventID == EVT_ID_SW1_PRESSED) || (eventID == EVT_ID_B1_PRESSED))
			return true;

		return false;

}

static bool EmergencyGuard(StateTableEntry_t * pEntry, int32_t eventID)
{
	if((eventID == EVT_ID_TRIGGER_EMERGENCY))
			return true;

		return false;
}

static bool TestModeGuard(StateTableEntry_t * pEntry, int32_t eventID)
{
	if((eventID == EVT_ID_SW2_PRESSED))
			return true;

		return false;
}

static bool FailureGuard(StateTableEntry_t *pEntry, int32_t eventID)
{
	if( ((eventID == EVT_ID_ERROR) ||  (eventID == EVT_ID_STACK_CORRUPTION) || (eventID == EVT_ID_SENSOR_DEFECT)) )
	{
		if(eventID == EVT_ID_SENSOR_DEFECT)
				sensorDefect = true;
		return true;
	}
	return false;
}








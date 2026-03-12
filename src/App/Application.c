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

#include "Application.h"

#include "UARTModule.h"

#include "HMI/ButtonHandler.h"

#include "HMI/LEDHandler.h"

#include "HMI/DisplayHandler.h"

#include "DualChannelGas.h"

#include "WaterSensor.h"

#include "Util/StateTable/StateTable.h"

#include "TimerModule.h"

#include "ADCModule.h"

#include "System.h"

#include "stm32g4xx_hal.h"

#include "AppContext.h"

#include "OnStates/AppOnStates.h"

#include "Guards/AppGuards.h"

#include "OnEntry/AppOnEntry.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/
#define GAS_SENSOR_WARNING_THRESHHOLD 3000

#define GAS_SENSOR_EMERGENCY_THRESHHOLD 5000

#define TEN_SEC_THRESHOLD_50MS 10000

#define FIVE_SEC_THRESHOLD_50MS 5000

#define THREE_SEC_THRESHOLD_50MS 3000

#define WATER_SENSOR_WARNING_THRESHHOLD 250 //in cm

#define WATER_SENSOR_EMERGENCY_THRESHHOLD 300

#define MAX_DISPLAY_NUMBER 999



/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/
static void changeVectorTableReset(void);

static int32_t initializePeripherals(void);

static int32_t monitorSensor(int32_t value, SensorMonitor_t *sensor);

static void initApplicationContext(void);

/***** PRIVATE VARIABLES *****************************************************/


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

	{STATE_ID_OPERATIONAL, 			STATE_ID_TESTMODE, 					EVT_ID_SW2_PRESSED, 			TestModeGuard,		&gStateList[2],      	&gStateList[4]},

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
//static StateTable_t gStateTable;
//
//static bool warningMode = false;
//
//static bool sensorDefect = false;

static ApplicationContext_t context = {0};




/***** PUBLIC FUNCTIONS ******************************************************/

int32_t AppInitialize(void)
{


	changeVectorTableReset();

	HAL_Init();

	SystemClock_Config();

	initializePeripherals();


	AppContext_Set(&context);

	int32_t initResult = AppOnStates_Init();

	if(initResult != APP_ON_STATE_OK)
		return APP_INIT_ERR;

	initResult = AppGuard_Init();

	if(initResult != APP_GUARD_ERR_OK )
		return APP_INIT_ERR;

	initResult = AppOnEntry_Init();
	if(initResult != APP_ON_ENTRY_OK )
			return APP_INIT_ERR;


	context.stateTable.pStateList = gStateList;

	context.stateTable.stateCount = sizeof(gStateList) / sizeof(State_t);

	int32_t tableInitialized = stateTableInitialize(&context.stateTable,
			gStateTableEntries,
			sizeof(gStateTableEntries) / sizeof(StateTableEntry_t),
			STATE_ID_INIT);

	if((tableInitialized == STATETBL_ERR_INVALID_PTR))
	{
		return APP_INIT_ERR;
	}

    initApplicationContext();
	//Initialize gasSensor Modul and check if it is ok
	int32_t dualGasInitRes = dualGasInit();

	if(dualGasInitRes != DUALSENSORS_OK)
	{
		AppSendEvent(EVT_ID_ERROR);
		return APP_NO_ERR;
	}


	//Initialize WaterSensor Modul and check if it is ok
	int32_t waterInitRes = waterSensorInitalize();

	if(waterInitRes != WATER_SENSOR_OK)
	{
		AppSendEvent(EVT_ID_ERROR);
		return APP_NO_ERR;
	}

    return APP_NO_ERR;
}

int32_t AppRun(void)
{
    int32_t StateTableResult = stateTableRunCyclic(&context.stateTable);

    if(StateTableResult == STATETBL_ERR_INVALID_PTR)
    {
    	return APP_RUN_ERR;
    }

    return APP_NO_ERR;
}

int32_t AppSendEvent(int32_t eventID)
{

    int32_t result = stateTableSendEvent(&context.stateTable, eventID);

    return result;
}


//priority 1-> B1 because reset, 2-> SW2, 3-> SW1
int32_t AppPollForButtonEvent(void)
{

	if(ButtonHandlerhasDebounced(BTN_B1))
	{
	    return EVT_ID_B1_PRESSED;
	}
	if(ButtonHandlerhasDebounced(BTN_SW2))
	{
	    return EVT_ID_SW2_PRESSED;
	}
	if(ButtonHandlerhasDebounced(BTN_SW1))
	{
	    return EVT_ID_SW1_PRESSED;
	}
	return NO_EVT;
}



int32_t AppUpdatingSensors()
{
	if(context.stateTable.currentStateID == STATE_ID_OPERATIONAL )
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

			event = monitorSensor(currentAverage, &context.gasSensor);
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


			DisplayHandlerSetDigits(currentWaterLevel);

			event = monitorSensor(currentWaterLevel, &context.waterSensor);
			if(event != NO_EVT)
			{
				return event;
			}

			if(context.gasSensor.warningLedTriggered == false && context.waterSensor.warningLedTriggered == false)
			{
				context.warningMode =false;
			}
	}

	return NO_EVT;
}


/***** PRIVATE FUNCTIONS *****************************************************/


static void changeVectorTableReset(void)
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

    int32_t checkInit = LEDHandler_Init();
    if(checkInit != LH_ERR_OK)
    	return APP_INIT_ERR;

    checkInit = DisplayHandlerInit();
    if(checkInit != DH_ERR_OK)
        	return APP_INIT_ERR;

    checkInit =ButtonHandlerInit();
    if(checkInit != BH_ERR_OK)
    	return APP_INIT_ERR;
    // Initialize Timer, DMA and ADC for sensor measurements
    int32_t timerInitRes = timerInitialize();
    if(timerInitRes != TIMER_ERR_OK)
    {
    	return APP_INIT_ERR;
    }

    adcInitialize();

    return APP_NO_ERR;

}

static int32_t monitorSensor(int32_t value, SensorMonitor_t *sensor)
{

	if(sensor == NULL)
	{
		return EVT_ID_ERROR;
	}

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
        sensor->warningLedTriggered = true; // @suppress("Symbol is not resolved")
        context.warningMode =true;
    }

    if(sensor->elapsedEmergencyTime >= sensor->emergencyTime)
    {
        sensor->elapsedEmergencyTime = 0;
        return EVT_ID_TRIGGER_EMERGENCY;
    }

    return NO_EVT;
}

static void initApplicationContext(void)
{
    context.gasSensor = (SensorMonitor_t){
        .warningThreshold = GAS_SENSOR_WARNING_THRESHHOLD,
        .emergencyThreshold = GAS_SENSOR_EMERGENCY_THRESHHOLD,
        .warningTime = FIVE_SEC_THRESHOLD_50MS,
        .emergencyTime = THREE_SEC_THRESHOLD_50MS,
        .elapsedWarningTime = 0U,
        .elapsedEmergencyTime = 0U,
        .lastTick = 0U,
        .warningLedTriggered = false
    };

    context.waterSensor = (SensorMonitor_t){
        .warningThreshold = WATER_SENSOR_WARNING_THRESHHOLD,
        .emergencyThreshold = WATER_SENSOR_EMERGENCY_THRESHHOLD,
        .warningTime = TEN_SEC_THRESHOLD_50MS,
        .emergencyTime = FIVE_SEC_THRESHOLD_50MS,
        .elapsedWarningTime = 0U,
        .elapsedEmergencyTime = 0U,
        .lastTick = 0U,
        .warningLedTriggered = false
    };
}










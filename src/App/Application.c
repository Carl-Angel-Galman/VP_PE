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

#include "Util/Global.h"

#include "Util/Log/printf.h"


#include "UARTModule.h"

#include "ButtonModule.h"

#include "LEDModule.h"

#include "DisplayModule.h"

#include "DualChannelGas.h"

#include "WaterSensor.h"

#include "Util/StateTable/StateTable.h"

#include "ADCModule.h"

#include "TimerModule.h"

#include "System.h"



/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/
#define GAS_SENSOR_WARNING_THRESHHOLD 3000

#define GAS_SENSOR_EMERGENCY_THRESHHOLD 5000

#define TEN_SEC_THRESHOLD_50MS 200

#define FIVE_SEC_THRESHOLD_50MS 100

#define THREE_SEC_THRESHOLD_50MS 60

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

static void change_vector_table(void);

static int32_t initializePeripherals(void);


/***** PRIVATE VARIABLES *****************************************************/

static uint32_t gasSensorWarningCount = 0;

static uint32_t waterSensorWarningCount = 0;

static uint32_t gasSensorEmergencyCount = 0;

static uint32_t waterSensorEmergencyCount = 0;


/**
 * @brief List of State for the State Machine
 *
 * This list only constructs the state objects for each possible state
 * in the state machine. There are no transistions or events defined
 *
 */

static State_t gStateList[] =
{
		{STATE_ID_INIT, 			displayDashOnEntry,  				onInit,                  	0,              false},

		{STATE_ID_PREOPERATIONAL, 	preOperationOnEntry,  				onPreOperational,           0,              false},

		{STATE_ID_OPERATIONAL, 		operationOnEntry,       			onOperational, 	    		0,  			false},

		{STATE_ID_FAILURE, 			failureOnEntry,  					0,                 			0,              false},

		{STATE_ID_TESTMODE, 		testModeOnEntry,  				    0,                  		0,              false},

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
	{STATE_ID_INIT, 			STATE_ID_PREOPERATIONAL,          	EVT_ID_INIT_READY,          	PreOpGuard,      	&gStateList[0],      	&gStateList[1]},

	{STATE_ID_INIT,          	STATE_ID_FAILURE,           		EVT_ID_ERROR,       	        FailureGuard,      	&gStateList[0],      	&gStateList[3]},

	{STATE_ID_PREOPERATIONAL, 	STATE_ID_OPERATIONAL,           	EVT_ID_SW1_PRESSED,      		OpGuard,      		&gStateList[1],      	&gStateList[2]},

	{STATE_ID_OPERATIONAL, 		STATE_ID_PREOPERATIONAL, 			EVT_ID_SW1_PRESSED, 			PreOpGuard,			&gStateList[2],      	&gStateList[1]},

	{STATE_ID_OPERATIONAL, 		STATE_ID_FAILURE, 					EVT_ID_ERROR, 					FailureGuard,		&gStateList[2],      	&gStateList[3]},

	{STATE_ID_OPERATIONAL, 		STATE_ID_EMERGENCY, 				EVT_ID_TRIGGER_EMERGENCY, 		EmergencyGuard,		&gStateList[2],      	&gStateList[5]},

	{STATE_ID_OPERATIONAL, 		STATE_ID_TESTMODE, 					EVT_ID_SW2_PRESSED, 			TestModeGuard,		&gStateList[2],      	&gStateList[5]},

	{STATE_ID_EMERGENCY, 		STATE_ID_OPERATIONAL, 				EVT_ID_ALARM_RESET, 			OpGuard,			&gStateList[0],      	&gStateList[2]}
};

/**
 * @brief Global State Table instance
 *
 */
static StateTable_t gStateTable;

static int8_t leftDigit = DIGIT_DASH;

static int8_t rightDigit = DIGIT_DASH;


/***** PUBLIC FUNCTIONS ******************************************************/

int32_t AppInitialize(void)
{
	change_vector_table();

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
	Button_Status_t sw1Status =  buttonGetButtonStatus(BTN_SW1);
	Button_Status_t sw2Status =  buttonGetButtonStatus(BTN_SW2);
	Button_Status_t b1Status =  buttonGetButtonStatus(BTN_B1);


	if(sw1Status == BUTTON_PRESSED)
	{
		return EVT_ID_SW1_PRESSED;
	}
	if(sw2Status == BUTTON_PRESSED)
	{
		return EVT_ID_SW2_PRESSED;
	}
	if(b1Status == BUTTON_PRESSED)
	{
		return EVT_ID_ALARM_RESET;
	}
	return NO_EVT;
}

int32_t AppDisplayDigitsOnSegments(void)
{
	displayTwoDigits(leftDigit, rightDigit);
	return APP_NO_ERR;
}


/***** PRIVATE FUNCTIONS *****************************************************/


static void change_vector_table(void)
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

static int32_t initOnEntry(State_t* pState, int32_t eventID)
{


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

	int32_t currentAverage = 0;
	int32_t currentWaterLevel = 0;

	gasSensorWarningCount++;

	gasSensorEmergencyCount++;

	waterSensorWarningCount++;

	waterSensorEmergencyCount++;

	if(dualGasSetVoltages() != DUALSENSORS_OK)
	{
		stateTableResult = stateTableSendEvent(&gStateTable, EVT_ID_ERROR);
		return STATETBL_ERR_OK;
	}
	if(dualGasCheckInconsistency() == DUALSENSORS_DEFECT)
	{
		stateTableResult = stateTableSendEvent(&gStateTable, EVT_ID_ERROR);
		return STATETBL_ERR_OK;
	}
	if(dualGasGetAverage(&currentAverage) != DUALSENSORS_OK)
	{
		stateTableResult = stateTableSendEvent(&gStateTable, EVT_ID_ERROR);
		return STATETBL_ERR_OK;
	}

	if(currentAverage <= GAS_SENSOR_WARNING_THRESHHOLD)
	{
		gasSensorWarningCount = 0;
	}

	if(currentAverage <=  GAS_SENSOR_EMERGENCY_THRESHHOLD)
	{
		gasSensorEmergencyCount = 0;
	}

	if(COUNTER_HAS_REACHED_FIVE_SECS_50MS(gasSensorWarningCount))
	{
		ledSetLED(LED1, LED_ON);
	}

	if(COUNTER_HAS_REACHED_THREE_SECS_50MS(gasSensorEmergencyCount))
	{
		gasSensorEmergencyCount =0 ;
		stateTableResult = stateTableSendEvent(&gStateTable, EVT_ID_TRIGGER_EMERGENCY);
		return STATETBL_ERR_OK;
	}

	//Warning, Emergency and Failure Logic for the watersensor
	if(waterSensorSetSensorVoltage() != WATER_SENSOR_OK)
	{
		stateTableResult = stateTableSendEvent(&gStateTable, EVT_ID_ERROR);
		return STATETBL_ERR_OK;
	}

	if(waterSensorGetSensorValue(&currentWaterLevel) != WATER_SENSOR_OK)
	{
		stateTableResult = stateTableSendEvent(&gStateTable, EVT_ID_ERROR);
		return STATETBL_ERR_OK;
	}

	if(currentWaterLevel <= WATER_SENSOR_WARNING_THRESHHOLD)
	{
		waterSensorWarningCount = 0;
	}

	if(currentWaterLevel <= WATER_SENSOR_EMERGENCY_THRESHHOLD)
	{
		waterSensorEmergencyCount = 0;
	}
	if(currentWaterLevel > MAX_DISPLAY_NUMBER)
	{
		currentWaterLevel = MAX_DISPLAY_NUMBER;
	}
	leftDigit = currentWaterLevel/HUNDREDS_DIGIT;
	rightDigit = currentWaterLevel/TENS_DIGIT;

	if(COUNTER_HAS_REACHED_TEN_SECS_50MS(waterSensorWarningCount))
	{
		ledSetLED(LED1, LED_ON);
	}

	if(COUNTER_HAS_REACHED_FIVE_SECS_50MS(waterSensorEmergencyCount))
	{
		waterSensorEmergencyCount = 0;
		stateTableResult = stateTableSendEvent(&gStateTable, EVT_ID_TRIGGER_EMERGENCY);
		return STATETBL_ERR_OK;
	}


	return stateTableResult;
}

static int32_t onEmergency(State_t *pState, int32_t eventID)
{
	ledToggleLED(LED1);

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
	ledSetLED(LED0,LED_OFF);
	leftDigit = DIGIT_DASH;
		rightDigit = DIGIT_DASH;
	return STATETBL_ERR_OK;
}

static int32_t testModeOnEntry(State_t *pState, int32_t eventID)
{
	ledSetLED(LED3,LED_ON);
	leftDigit = DIGIT_DASH;
	rightDigit = DIGIT_DASH;
	return STATETBL_ERR_OK;
}

static int32_t operationOnEntry(State_t *pState, int32_t eventID)
{
	ledSetLED(LED0,LED_ON);
	return STATETBL_ERR_OK;
}

static int32_t failureOnEntry(State_t *pState, int32_t eventID)
{
	ledTurnOnAllLEDs();
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
	if((eventID == EVT_ID_SW1_PRESSED) || (eventID == EVT_ID_ALARM_RESET))
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
	if((eventID ==STATE_ID_FAILURE) ||  (eventID == EVT_ID_ERROR))
	{
		return true;
	}
	return false;
}







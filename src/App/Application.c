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


#include "Util/StateTable/StateTable.h"

#include "System.h"



/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/
typedef struct
{
    Button_t button;
    Button_Status_t state;
} ButtonEvent_t;

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

	// Initialize the System Clock
	SystemClock_Config();

	initializePeripherals();

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
		return EVT_ID_B1_PRESSED;
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

    //displayInitialize();

    // Initialize GPIOs for Buttons
    buttonInitialize();
    // Initialize Timer, DMA and ADC for sensor measurements
//    timerInitialize();
//
//    adcInitialize();

    return APP_NO_ERR;

}

static int32_t initOnEntry(State_t* pState, int32_t eventID)
{


	return STATETBL_ERR_OK;
}

static int32_t onInit(State_t* pState, int32_t eventID)
{
	// check gas Sensor
    bool validValues = true;
    if(validValues)
    {
        int32_t result = stateTableSendEvent(&gStateTable, EVT_ID_INIT_READY);
    }

    else
    {
        int32_t result = stateTableSendEvent(&gStateTable, EVT_ID_INIT_READY);

    }

	return STATETBL_ERR_OK;
}

static int32_t onPreOperational(State_t * pState, int32_t eventID)
{

	return STATETBL_ERR_OK;
}

static int32_t onOperational(State_t * pState, int32_t eventID)
{

	leftDigit = 6;
	rightDigit = 7;


	return STATETBL_ERR_OK;
}

static int32_t onEmergency(State_t *pState, int32_t eventID)
{

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







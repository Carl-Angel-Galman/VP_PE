/******************************************************************************
 * @file main.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Main file for the VP Template Authenticator project
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "stm32g4xx_hal.h"

#include "System.h"

#include "HardwareConfig.h"

#include "Util/Global.h"
#include "Util/Log/printf.h"
#include "Util/Log/LogOutput.h"

#include "UARTModule.h"
#include "ButtonModule.h"
#include "LEDModule.h"
#include "DisplayModule.h"
#include "ADCModule.h"
#include "TimerModule.h"
#include "Scheduler.h"
#include "StateTable.h"

#include "GlobalObjects.h"

#include "stdbool.h"

typedef enum
{
	BOOTUP = 0,
	PREPARE_APPLICATION = 1,
	FAILURE = 2,
	START_APPLICATION = 3
}State;


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/
static int32_t initializePeripherals();


static State current_state = BOOTUP;



/***** PRIVATE VARIABLES *****************************************************/
static Scheduler gScheduler;            // Global Scheduler instance



/***** PUBLIC FUNCTIONS ******************************************************/


/**
 * @brief Main function of System
 */
int main(void)
{


    // Initialize Scheduler


    while (1)
    {
    	swtich(current_state)
		{
    		case BOOTUP:
    			// Initialize the HAL
				if(HAL_Init() != HAL_OK)
				{
					current_state = FAILURE;
				}

				SystemClock_Config();

				// Initialize Peripherals
				if(initializePeripherals() != ERROR_OK)
				{
					current_state = FAILURE;
				}

				current_state = PREPARE_APPLICATION;
    			break;
    		case PREPARE_APPLICATION:
    		    schedInitialize(&gScheduler);
    		    // check for description

    		    if(descritp)
    		    {
    		    	current_state = START_APPLICATION;
    		    }
    		    if(timeout)
    		    {
    		    	current_state = FAILURE;
    		    }

    			break;
    		case FAILURE:
    			break;
    		case START_APPLICATION:

    			__set_MSP();
    			SCB->vector = StartOfStackAdress
    			break;
    		default:
    			break;
		}


    }
}

/***** PRIVATE FUNCTIONS *****************************************************/

/**
 * @brief Initializes the used peripherals like GPIO,
 * ADC, DMA and Timer Interrupts
 *
 * @return Returns ERROR_OK if no error occurred
 */
static int32_t initializePeripherals()
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

    return ERROR_OK;
}




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
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_rcc.h"


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

#include "GlobalObjects.h"

#include "stdbool.h"

#include "Authenticator.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/
typedef enum
{
	BOOTUP = 0,
	PREPARE_APPLICATION = 1,
	FAILURE = 2,
	START_APPLICATION = 3
}State;

/***** PRIVATE PROTOTYPES ****************************************************/
static int32_t initializePeripherals();


static State current_state = BOOTUP;


/***** PRIVATE VARIABLES *****************************************************/



/***** PUBLIC FUNCTIONS ******************************************************/


/**
 * @brief Main function of System
 */
int main(void)
{
	while(1)
	{
	switch(current_state)
	{

		case BOOTUP:
			// Initialize the HAL
			HAL_Init();

			SystemClock_Config();

			// Initialize Peripherals
			initializePeripherals();

			current_state = PREPARE_APPLICATION;
			break;

		case PREPARE_APPLICATION:

			int8_t res = Auth_WaitForA();

			if(res == AUTH_ERR_TIMEOUT)
			{
				current_state = FAILURE;
			}else{
				current_state = START_APPLICATION;
			}
<<<<<<< HEAD
			current_state = START_APPLICATION;
=======
>>>>>>> c82fa012ab0d65be25b6dd973cb5feed14ecad17


			break;

		case FAILURE:

			while(1);

			break;
		case START_APPLICATION:

			{
				uint8_t key_len = 8U;

				int8_t key[key_len];

				int8_t res = Auth_ReadKey(key, &key_len);

<<<<<<< HEAD
				//uint8_t key[] = "VP2026";
=======
>>>>>>> c82fa012ab0d65be25b6dd973cb5feed14ecad17
				copy_and_decrypt_auth_section(key);

				verify();

			}

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




















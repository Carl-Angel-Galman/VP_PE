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
			if(HAL_Init() != HAL_OK) break;

			SystemClock_Config();

			// Initialize Peripherals
			if(initializePeripherals() != ERR_OK) break;

			current_state = PREPARE_APPLICATION;

			break;

		case PREPARE_APPLICATION:
		{
			int8_t res = Auth_WaitForA();

			if(res == AUTH_ERR_TIMEOUT)
			{
				current_state = FAILURE;
				break;
			}

			uint8_t key_len = 8U;

			int8_t key[key_len];

			res = Auth_ReadKey(key, &key_len);

			if(res == AUTH_ERR_KEY_LENGHT_BREACH)
			{
				current_state = FAILURE;
				break;
			}

			copy_and_decrypt_auth_section(key);

			current_state = START_APPLICATION;

			}
			break;

		case FAILURE:

			while(1);

			break;

		case START_APPLICATION:
		{
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
    if(uartInitialize(115200) != UART_ERR_OK)	return AUTH_ERR_FAILURE;

    // Initialize GPIOs for LED and 7-Segment output
	if(ledInitialize()!= LED_ERR_OK) 			return AUTH_ERR_FAILURE ;
    if(displayInitialize()!= DISPLAY_ERR_OK) 	return AUTH_ERR_FAILURE;

    // Initialize GPIOs for Buttons
    if (buttonInitialize()!= BUTTON_ERR_OK) 	return AUTH_ERR_FAILURE;

    // Initialize Timer, DMA and ADC for sensor measurements
    if(timerInitialize()!= TIMER_ERR_OK) 		return AUTH_ERR_FAILURE;
    if(adcInitialize()!= ADC_ERR_OK) 			return AUTH_ERR_FAILURE;

    return ERROR_OK;
}




















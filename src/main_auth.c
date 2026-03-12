/******************************************************************************
 * @file main.c
 *
 * @author Carl Angel Galman and Liza Thöne
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Main entry point of the Authenticator application.
 *
 * @details
 * This module initializes the hardware and executes the main state machine of
 * the authenticator.
 *
 * The state machine performs the following steps:
 * - BOOTUP:
 *   Initializes all required peripherals and the Authenticator module.
 * - PREPARE_APPLICATION:
 *   Waits for the start character, reads the authentication key, and copies
 *   and decrypts the `.auth` section.
 * - START_APPLICATION:
 *   Executes the `verify()` function from the decrypted `.auth` section.
 * - FAILURE:
 *   Enters a permanent failure state.
 *
 * The implementation follows the project specification that only a timeout
 * while waiting for the start character leads directly to the FAILURE state.
 *
 *****************************************************************************/

/***** INCLUDES **************************************************************/
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_rcc.h"


#include "System.h"

#include "HardwareConfig.h"

#include "Util/Log/printf.h"
#include "Util/Log/LogOutput.h"

#include "UARTModule.h"
#include "LEDModule.h"
#include "TimerModule.h"


#include "stdbool.h"

#include "Authenticator.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/
#define BAUD_RATE 115200

/***** PRIVATE TYPES *********************************************************/



/**
 * @brief Initializes all peripherals required by the authenticator.
 *
 * @details
 * This function initializes the UART interface for debug and key input,
 * the LED module for status indication, and the timer module used by the
 * application.
 *
 * @retval AUTH_ERR_OK
 * All peripherals were initialized successfully.
 *
 * @retval AUTH_ERR_FAILURE
 * At least one peripheral initialization failed.
 */
/***** PRIVATE PROTOTYPES ****************************************************/
static int32_t initializePeripherals(void);



/***** PRIVATE VARIABLES *****************************************************/

/**
 * @brief Current state of the main authenticator state machine.
 */
static State current_state = BOOTUP;


/***** PUBLIC FUNCTIONS ******************************************************/


/**
 * @brief Main function of the authenticator application.
 *
 * @details
 * Initializes the HAL and system clock and then executes the authenticator
 * state machine forever.
 *
 * State flow:
 * BOOTUP -> PREPARE_APPLICATION -> START_APPLICATION
 * or
 * BOOTUP -> PREPARE_APPLICATION -> FAILURE
 *
 * @return This function never returns.
 */
int main(void)
{
	HAL_Init();

	SystemClock_Config();

	while(1)
	{

		switch(current_state)
		{

			case BOOTUP:

				if(initializePeripherals() != AUTH_ERR_OK)
					break;

				AuthInit();

				current_state = PREPARE_APPLICATION;

				break;

			case PREPARE_APPLICATION:
				{
					int8_t keyReadResult = AuthWaitForA();

					if(keyReadResult == AUTH_ERR_TIMEOUT)
					{
						AuthGoToFailure();

						current_state = FAILURE;
						break;
					}
					else if(keyReadResult == AUTH_ERR_FAILURE)
					{
						break;
					}

					uint8_t key_len = 0U;

					uint8_t key[MAX_KEY_LEN] = {0u};

					keyReadResult = AuthReadKey(key, &key_len);

					if(keyReadResult == AUTH_ERR_TIMEOUT)
					{
						AuthGoToFailure();

						current_state = FAILURE;
						break;
					}

					int32_t copyAndDecryptResult = AuthCopyAndDecryptVerify(key, key_len);

					if(copyAndDecryptResult == AUTH_ERR_INVALID_PTR)
					{
						break;
					}

					current_state = START_APPLICATION;
				}

				break;

			case FAILURE:

				while(1)
				{

				}
				break;

			case START_APPLICATION:

				AuthGoToApplicationStart();

				verify();

			break;

			default:
				break;
		}

	}
}

/***** PRIVATE FUNCTIONS *****************************************************/

/**
 * @brief Initializes the peripherals used by the authenticator.
 *
 * @details
 * Initializes UART for communication, LEDs for status indication, and the
 * timer module used by the system.
 *
 * @retval AUTH_ERR_OK
 * Initialization completed successfully.
 *
 * @retval AUTH_ERR_FAILURE
 * Initialization of at least one peripheral failed.
 */
static int32_t initializePeripherals(void)
{
    // Initialize UART used for Debug-Outputs
    if(uartInitialize(BAUD_RATE) != UART_ERR_OK)
    	return AUTH_ERR_FAILURE;

    // Initialize GPIOs for LED and 7-Segment output
	if(ledInitialize()!= LED_ERR_OK)
		return AUTH_ERR_FAILURE;

    // Initialize Timer, DMA and ADC for sensor measurements
    if(timerInitialize()!= TIMER_ERR_OK)
    	return AUTH_ERR_FAILURE;

    return AUTH_ERR_OK;
}




















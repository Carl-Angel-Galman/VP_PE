/******************************************************************************
 * @file Authenticator.c
 *
 * @author Carl Angel Galman and Liza Thöne
 * @date   08.03.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Implementation of the Authenticator module.
 *
 * @details
 * The Authenticator is responsible for preparing and starting the UMMS
 * application. After boot, it waits for the start character 'A' on UART,
 * receives a decryption key with a maximum length of 8 bytes, copies the
 * encrypted `.auth` section from FLASH to RAM, decrypts it using a byte-wise
 * XOR operation, and finally executes the `verify()` function from RAM.
 *
 * The `verify()` function checks the application signature stored in FLASH and,
 * if valid, transfers control to the application's start handler.
 *
 * The timeout behavior during key reception is:
 * - after 10 s: LED D1 on
 * - after 30 s: LED D1 flashing
 * - after 45 s: authentication failure
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/

#include "Authenticator.h"

#include <string.h>

#include "UARTModule.h"

#include "stdbool.h"

#include "LEDModule.h"

#include "Scheduler.h"

#include "Util/Global.h"

#include "Util/Log/printf.h"

#include "Util/Log/LogOutput.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


#define WAIT_A_TIMEOUT_MS     		15000u

#define KEY_WARNING_STAGE1_MS       10000u

#define KEY_WARNING_STAGE2_MS       30000u

#define KEY_FAIL_MS           		45000u

#define APP_STARTHANDLER_ADDR  		0x08010204u

#define APP_SIGNATURE_ADDR 			0x08010000u

#define RECEIVE_CHARACTER 			(uint8_t)('A')

#define NEWLINE_CHARACTER 			'\n'

#define KEY_POLL_TIMEOUT_MS 		20u

#define IS_A_LETTER_OR_NUMBER(ch) 	(ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')

#define SIZE_OF_SIGNATURE 			4u

#define BLINKY_TIME_THRESHOLD 		500u

#define MIN_KEY_LEN 1u

/***** PRIVATE TYPES *********************************************************/

typedef void (*app_start_function)(void);

typedef enum
{
	INITIAL = 0,

	FIRST_WARNING = 1,

	SECOND_WARNING = 2,

	TIMEOUT = 3

}KEY_INPUT_WARNING_STAGES;


/***** PRIVATE PROTOTYPES ****************************************************/
static void Flash_D1(uint32_t elapsedTime) ;

static void keyReadingWarningDetermination(uint32_t elapsed);


/*******************************************************************************
 * Global Variables
 ******************************************************************************/

extern uint8_t _sloadauth;

extern uint8_t _sauth;

extern uint8_t _eauth;

/***** PRIVATE VARIABLES *****************************************************/

static KEY_INPUT_WARNING_STAGES keyInputWarningStage;

static const char ExpectedAppSignature[] = "UMMS";


/***** PUBLIC FUNCTIONS ******************************************************/

/**
 * @brief Verifies the application signature and starts the application.
 *
 * @details
 * This function is linked into the `.auth` section. The section is stored in
 * FLASH and copied to RAM before execution. After decryption, this function
 * compares the expected application signature with the signature stored at the
 * fixed application signature address in FLASH.
 *
 * If the signature is valid, interrupts are disabled and control is transferred
 * to the application start handler via a function pointer.
 *
 * @note
 * This function must only be called after the `.auth` section has been copied
 * from FLASH to RAM and decrypted successfully.
 *
 * @warning
 * If the signature check fails, this function does not return and remains in an
 * infinite loop.
 *
 * @return None.
 */
__attribute__((section(".auth") , noinline))
void verify(void)
{

	if(memcmp((const char*)APP_SIGNATURE_ADDR, ExpectedAppSignature, SIZE_OF_SIGNATURE) == 0)
	{

		outputLog("[AUTH]: Starting App");

		__disable_irq();

		uint32_t *start_app_ptr = (uint32_t *)(APP_STARTHANDLER_ADDR);

		app_start_function start = (app_start_function) *(start_app_ptr);

		start();

	}

	// a super loop to in-case the memcopy goes to failure
	while (1) { }

}


/**
 * @brief Copies the `.auth` section from FLASH to RAM and decrypts it.
 *
 * @details
 * This function uses the linker symbols `_sloadauth`, `_sauth`, and `_eauth`
 * to determine the source address in FLASH, the destination address in RAM,
 * and the section size. After copying, the RAM image is decrypted in place
 * using a byte-wise XOR with the provided key.
 *
 * A data synchronization barrier and instruction synchronization barrier are
 * executed afterwards to ensure that the modified RAM contents are visible
 * before code execution continues.
 *
 * @param[in] key
 * Pointer to the decryption key buffer.
 *
 * @param[in] key_len
 * Length of the decryption key in bytes. Must be greater than 0.
 *
 * @return AUTH_ERR_OK
 * The `.auth` section was copied and decrypted successfully.
 *
 * @return AUTH_ERR_INVALID_PTR
 * `key` is `NULL` or `key_len` is 0.
 */
int8_t AuthCopyAndDecryptVerify(uint8_t key[], uint8_t key_len)
{
    if(key == NULL || key_len == 0)
        return AUTH_ERR_INVALID_PTR;

    uint8_t *dst = &_sauth;
    uint8_t *src = &_sloadauth;

    size_t section_len = (size_t)(&_eauth - &_sauth);

    memcpy(dst, src, section_len);

    for(size_t i = 0; i < section_len; i++)
    {
        dst[i] ^= key[i % key_len];
    }

    __DSB();
    __ISB();

    return AUTH_ERR_OK;
}

/**
 * @brief Waits for the authenticator start character on UART.
 *
 * @details
 * This function waits up to 15 seconds for the character `'A'`. Reception of
 * this character starts the key input phase of the authenticator.
 *
 * @retval AUTH_ERR_OK
 * The expected start character was received.
 *
 * @retval AUTH_ERR_TIMEOUT
 * No valid start character was received within the 15 second timeout period.
 *
 * @retval AUTH_ERR_FAILURE
 * Another UART failure has occured that has not been considered.
 */
int8_t AuthWaitForA(void)
{
	uint8_t charBuffer = 0 ;

	uint32_t startTimeStamp = HAL_GetTick();

	uint32_t elapsed = 0;

	bool hasData = 0;

	int32_t hasDataResult = 0;


	while(1)
	{
		uint32_t currentTime = HAL_GetTick();

		elapsed = currentTime - startTimeStamp;

		if (elapsed >=  WAIT_A_TIMEOUT_MS)
		{
			outputLogf("[AUTH] going to timeout, with elapsed: %d \n", (uint32_t)elapsed/1000);

			return AUTH_ERR_TIMEOUT;

		}

		hasDataResult = uartHasData(&hasData);

		if((hasDataResult == UART_ERR_OK) && (hasData == true))
		{

			int32_t uartReceiveResult = uartReceiveData(&charBuffer, 1, KEY_POLL_TIMEOUT_MS);

			if(uartReceiveResult == UART_ERR_OK)
			{

				if(charBuffer == RECEIVE_CHARACTER)
				{

					outputLogf("%c \r", NEWLINE_CHARACTER);

					return AUTH_ERR_OK ;
				}
			}
		}
	}

    return AUTH_ERR_FAILURE;
}

/**
 * @brief Reads the decryption key from UART.
 *
 * @details
 * This function reads an ASCII key from UART until a newline character is
 * received or an error condition occurs. Only alphanumeric characters are
 * accepted. Invalid characters are ignored. The maximum supported key length
 * is 8 bytes.
 *
 * During key reception, the timeout warning state is updated as follows:
 * - after 10 s: first warning
 * - after 30 s: second warning
 * - after 45 s: timeout/failure
 *
 * @param[out] key
 * Destination buffer for the received key bytes.
 *
 * @param[out] keylen
 * Pointer to the variable that receives the actual key length.
 *
 * @retval AUTH_ERR_OK
 * A complete key terminated by newline was received successfully.
 *
 * @retval AUTH_ERR_INVALID_PTR
 * `key` or `keylen` is `NULL`.
 *
 * @retval AUTH_ERR_TIMEOUT
 * Key reception timed out.
 *
 * @retval AUTH_ERR_KEY_LENGHT_BREACH
 * The received key exceeded the maximum supported length.
 *
 * @retval AUTH_ERR_FAILURE
 * UART reception failed.
 */
int8_t AuthReadKey(uint8_t key[], uint8_t *keylen)
{

	if(key == NULL || keylen == NULL)
	{
		return AUTH_ERR_INVALID_PTR;
	}

    uint32_t start = HAL_GetTick();

    uint32_t now = 0;

    uint32_t elapsed = 0;

    uint8_t ch = 0u;

	uint8_t len = 0u;

	int32_t hasDataResult = 0;

	bool hasData = false;

	ledSetLED(LED1, LED_OFF);

	outputLog("[AUTH] Please enter your key: ");

    while (1)
    {

        now = HAL_GetTick();

        elapsed = now - start;

        keyReadingWarningDetermination(elapsed);

        if(keyInputWarningStage ==TIMEOUT)
		{

        	return AUTH_ERR_TIMEOUT;
		}

        hasDataResult = uartHasData(&hasData);

		if((hasDataResult == UART_ERR_OK) && (hasData == true))
		{

            int32_t r = uartReceiveData(&ch, 1, KEY_POLL_TIMEOUT_MS);

            if (r == UART_ERR_RECEIVE)
			{

				return AUTH_ERR_FAILURE;
			}


			else if((ch == (uint8_t)NEWLINE_CHARACTER) && len >= MIN_KEY_LEN)
			{
				*keylen = len;

				return AUTH_ERR_OK;
			}

			else if (len < MAX_KEY_LEN)
			{
				if (IS_A_LETTER_OR_NUMBER(ch))
				{
					  key[len++] = ch;
				}
			}
			else if(len >= MAX_KEY_LEN)
			{
			// do nothing.
			}
        }
        // implicitly none valid chars will be skipped.
	}

    return AUTH_ERR_FAILURE;
}

/**
 * @brief Initializes the Authenticator module state.
 *
 * @details
 * This function initializes the internal warning state machine and activates
 * LED D0 to indicate that the authenticator is active.
 *
 * @retval AUTH_ERR_OK
 * Initialization was completed successfully.
 */
int8_t AuthInit(void)
{

	keyInputWarningStage = INITIAL;

	ledSetLED(LED0, LED_ON);

	return AUTH_ERR_OK;
}

/**
 * @brief Enters the authenticator failure indication state.
 *
 * @details
 * This function activates LED D4 to indicate an authentication failure.
 *
 * @retval AUTH_ERR_OK
 * Failure indication was activated successfully.
 */
int8_t AuthGoToFailure(void)
{
	outputLog("[AUTH] Going to Failure. \n");

	ledSetLED(LED4, LED_ON);

	return AUTH_ERR_OK;
}


/***** PRIVATE FUNCTIONS *****************************************************/

/**
 * @brief Toggles LED D1 for visual timeout warning indication.
 *
 * @details
 * This helper function toggles LED D1 when the elapsed time has reached the
 * flashing phase threshold.
 *
 * @param[in] elapsedTime
 * Elapsed time in milliseconds since the start of key reception.
 *
 * @return None.
 */
static void Flash_D1(uint32_t elapsedTime)
{

	static uint32_t lastBlinkTime = 0;

	if((elapsedTime - lastBlinkTime) >= BLINKY_TIME_THRESHOLD)
	{
	    ledToggleLED(LED1);
	    lastBlinkTime = elapsedTime;
	}
}


/**
* @brief Updates the key input warning state based on elapsed time.
*
* @details
* This function implements the timeout warning behavior of the authenticator:
* - INITIAL: no warning active
* - FIRST_WARNING: entered after 10 s, LED D1 is turned on
* - SECOND_WARNING: entered after 30 s, LED D1 starts flashing
* - TIMEOUT: entered after 45 s, authentication fails
*
* @param[in] elapsed
* Elapsed time in milliseconds since the start of key reception.
*
* @return None.
*/
static void keyReadingWarningDetermination(uint32_t elapsed)
{
    switch(keyInputWarningStage)
    {

		case INITIAL:

			if (elapsed >= KEY_WARNING_STAGE1_MS)
			{
				outputLog("[AUTH]: 10 seconds have passed \n\r");

				ledSetLED(LED1, LED_ON);

				keyInputWarningStage = FIRST_WARNING;

			}

			break;

		case FIRST_WARNING:

			if (elapsed >= KEY_WARNING_STAGE2_MS)
			{
				outputLog("[AUTH]: 30 seconds have passed \n");

				keyInputWarningStage = SECOND_WARNING;
			}
			break;

		case SECOND_WARNING:

			Flash_D1(elapsed);

			if (elapsed >= KEY_FAIL_MS)
			{
				ledSetLED(LED1, LED_OFF);

				keyInputWarningStage = TIMEOUT;
			}
			break;

		case TIMEOUT:

			break;
    }
}




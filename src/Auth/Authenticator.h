/******************************************************************************
 * @file Authenticator.h
 *
 * @author Carl Angel Galman and Liza Thöne
 * @date   08.03.2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief Public interface of the Authenticator module.
 *
 * @details
 * The Authenticator module is responsible for preparing and starting the
 * application securely.
 *
 * It waits for a start character via UART, reads a user key, copies the
 * encrypted `.auth` section from FLASH to RAM, decrypts it, and then executes
 * the `verify()` function from RAM.
 *
 * The `verify()` function checks whether the application signature stored in
 * FLASH matches the expected signature. If the signature is valid, execution
 * is transferred to the application start handler.
 *
 *****************************************************************************/

#ifndef _AUTHENTICATOR_H
#define _AUTHENTICATOR_H

/***** INCLUDES **************************************************************/

#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_rcc.h"

/***** CONSTANTS *************************************************************/

/***** MACROS ****************************************************************/

/** @brief No error occurred. */
#define AUTH_ERR_OK                    0

/** @brief Initialization error occurred. */
#define AUTH_ERR_INIT                 -1

/** @brief General authenticator failure. */
#define AUTH_ERR_FAILURE              -2

/** @brief Operation timed out. */
#define AUTH_ERR_TIMEOUT              -3

/** @brief Invalid pointer argument was passed. */
#define AUTH_ERR_INVALID_PTR          -4

/** @brief Entered key exceeds the supported maximum length. */
#define AUTH_ERR_KEY_LENGHT_BREACH    -5


#define MAX_KEY_LEN 8u
/***** TYPES *****************************************************************/

/**
 * @brief Authenticator state machine states.
 */
typedef enum
{
    BOOTUP = 0,              /**< Initial boot state. */
    PREPARE_APPLICATION = 1, /**< Prepare application startup. */
    FAILURE = 2,             /**< Authentication failed. */
    START_APPLICATION = 3    /**< Application may be started. */
} State;

/***** PROTOTYPES ************************************************************/

/**
 * @brief Verifies the application signature and starts the application.
 *
 * @details
 * This function is placed in the `.auth` section and is intended to execute
 * from RAM after the section has been copied from FLASH and decrypted.
 *
 * It compares the expected application signature with the signature stored in
 * FLASH. If the signature is valid, interrupts are disabled and execution is
 * transferred to the application start handler.
 *
 * @warning
 * This function must only be called after the `.auth` section has been copied
 * to RAM and decrypted successfully.
 *
 * @return None.
 */
void verify(void) __attribute__((section(".auth"), noinline));

/**
 * @brief Copies the `.auth` section from FLASH to RAM and decrypts it.
 *
 * @param[in] key
 * Pointer to the key used for decryption.
 *
 * @param[in] keylen
 * Length of the key in bytes. Must be greater than 0.
 *
 * @retval AUTH_ERR_OK
 * The `.auth` section was copied and decrypted successfully.
 *
 * @retval AUTH_ERR_INVALID_PTR
 * `key` is `NULL` or `keylen` is 0.
 */
int8_t AuthCopyAndDecryptVerify(uint8_t key[], uint8_t keylen);

/**
 * @brief Waits for the start character on UART.
 *
 * @details
 * This function waits for the character `'A'` within a fixed timeout period.
 * Reception of this character enables the next authentication step.
 *
 * @retval AUTH_ERR_OK
 * The expected start character was received.
 *
 * @retval AUTH_ERR_TIMEOUT
 * The timeout expired before the expected character was received.
 *
 * @retval AUTH_ERR_FAILURE
 * A UART error occurred or an unexpected character was received.
 */
int8_t AuthWaitForA(void);

/**
 * @brief Reads the authentication key from UART.
 *
 * @details
 * Reads characters from UART until a newline character is received, a timeout
 * occurs, or the maximum key length is exceeded. Only alphanumeric characters
 * are accepted.
 *
 * @param[out] key
 * Buffer that receives the key bytes.
 *
 * @param[out] keylen
 * Pointer to a variable that receives the actual key length.
 *
 * @retval AUTH_ERR_OK
 * The key was read successfully.
 *
 * @retval AUTH_ERR_INVALID_PTR
 * `key` or `keylen` is `NULL`.
 *
 * @retval AUTH_ERR_TIMEOUT
 * Key input timed out.
 *
 * @retval AUTH_ERR_KEY_LENGHT_BREACH
 * The entered key exceeded the maximum allowed length.
 *
 * @retval AUTH_ERR_FAILURE
 * A UART receive error occurred.
 */
int8_t AuthReadKey(uint8_t key[], uint8_t *keylen);

/**
 * @brief Initializes the Authenticator module.
 *
 * @details
 * Resets the internal warning state and activates the initial authenticator
 * status indication.
 *
 * @retval AUTH_ERR_OK
 * Initialization was successful.
 */
int8_t AuthInit(void);

/**
 * @brief Activates the failure indication of the Authenticator.
 *
 * @details
 * This function switches the module into its visible failure indication state.
 *
 * @retval AUTH_ERR_OK
 * Failure indication was activated successfully.
 */
int8_t AuthGoToFailure(void);



int32_t AuthGoToApplicationStart(void);

#endif

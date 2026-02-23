/******************************************************************************
 * @file <Filename>.h
 *
 * @author <Author>
 * @date   <Date>
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief <Some short descrition>
 *
 * @details <A more detailed description>
 *
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
#define AUTH_ERR_OK                  0          //!< No error occured
#define AUTH_ERR_FAILURE        -1         //!< Error during UART initialization
#define AUTH_ERR_TIMEOUT		-2
#define AUTH_ERR_INVALID_PTR    -3

/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/
void verify(void) __attribute__((section(".auth"), used, noinline));

int8_t copy_and_decrypt_auth_section(uint8_t key[]);

int8_t Auth_WaitForA(void);

int8_t Auth_ReadKey(uint8_t key[8], uint8_t *outLen);

int8_t Auth_init(void);


#endif



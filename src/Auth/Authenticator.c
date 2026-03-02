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


#define APP_SIGNATURE_ADDR ((volatile const uint8_t*)0x08010000u)

#define WAIT_A_TIMEOUT_MS     (15000u)

#define KEY_STAGE1_MS         (10000u)   // D1 on
#define KEY_STAGE2_MS         (30000u)   // D1 flashing
#define KEY_FAIL_MS           (45000u)

#define KEY_MAX_LEN           (8u)

#define APP_STARTHANDLER_ADDR    (0x08010200)

/***** PRIVATE TYPES *********************************************************/



typedef void (*app_start_function)(void);

typedef enum
{
	INITIAL = 0,
	FIRST_WARNING = 1,
	SECOND_WARNING = 2,
	TIMEOUT = 3

}KEY_INPUT_STAGES;



/***** PRIVATE PROTOTYPES ****************************************************/
static void switch_to_app(void);

static void D1_FlashUpdate(int32_t nowMs);

static void Flash_D1(void) ;

/***** PRIVATE VARIABLES *****************************************************/

extern uint8_t _sloadauth;
extern uint8_t _sauth;
extern uint8_t _eauth;


static uint8_t sig_copy_in_RAM[4U];



static KEY_INPUT_STAGES key_input_stage = INITIAL;

static Scheduler auth_Scheduler;

// Linker-defined symbol. It is an address, not a variable.
/***** PUBLIC FUNCTIONS ******************************************************/
__attribute__((section(".auth"), used, noinline))
void verify(void)
{
	volatile uint8_t r0 = sig_copy_in_RAM[0];
	volatile uint8_t r1 = sig_copy_in_RAM[1];
	volatile uint8_t r2 = sig_copy_in_RAM[2];
	volatile uint8_t r3 = sig_copy_in_RAM[3];

	if((r0 == 'U' &&
		r1 == 'M' &&
		r2 == 'M' &&
		r3 == 'S'))
	{
		__disable_irq();


		uint32_t *start_app_ptr = (uint32_t *)(0x08010200 + 4);
		app_start_function start = (app_start_function) *(start_app_ptr);
		start();

	}


	while (1) { }

}



int8_t copy_and_decrypt_auth_section(uint8_t key[], uint8_t key_len)
{

	if(key == 0)
	{
		return AUTH_ERR_INVALID_PTR;
	}

	uint8_t  *dst = &_sauth;

    size_t section_len = (size_t)(&_eauth - &_sauth);

	const uint8_t * src = &_sloadauth;

    memcpy(dst, src, section_len);


    for (size_t i = 0; i < section_len; i++)
	{
		dst[i] ^= key[i % key_len];
	}

    __DSB();__ISB();



    return AUTH_ERR_OK;
}

int8_t Auth_WaitForA(void)
{
	uint8_t ch = 0 ;

	uint8_t toSend = '\n';
	int32_t r = uartReceiveData(&ch, 1, 15000u);
//    if(ch != 0) outputLogf("\r\x1b[K%c",ch);

	if (r == UART_ERR_OK)
	{
		if (ch == (uint8_t)'A')
		{
			uartSendData(&toSend, 1);

			return AUTH_ERR_OK ;
		}
		// ignore other chars
	}
	else if (r == UART_ERR_TIMEOUT)
	{
		return AUTH_ERR_TIMEOUT; // treat UART error as failure
	}


    return AUTH_ERR_FAILURE;
}


int8_t Auth_ReadKey(uint8_t key[], uint8_t *keylen)
{


	if(key == NULL || keylen == NULL)
	{
		return AUTH_ERR_INVALID_PTR;
	}
    uint32_t start = HAL_GetTick();
    uint32_t now;
    uint32_t elapsed;
    uint8_t ch = 0;

	ledSetLED(LED1, LED_OFF);

	uint8_t len = 0;

    while (1)
    {
        now = HAL_GetTick();
        elapsed = now - start;

        switch(key_input_stage)
        {

        case INITIAL:
        	if (elapsed >= 10000u)
        	{
        		ledSetLED(LED1, LED_ON);
        		key_input_stage = FIRST_WARNING;
        	}

        	break;

        case FIRST_WARNING:

        	if (elapsed >= 30000u)
        	{
        		auth_Scheduler.pTask_250ms = Flash_D1;
        		key_input_stage = SECOND_WARNING;
        	}
        	break;

        case SECOND_WARNING:
        	schedCycle(&auth_Scheduler);
        	if (elapsed >= 45000u)
        	{
        		ledSetLED(LED1, LED_OFF);
        		key_input_stage = TIMEOUT;
        	}
        	break;

        case TIMEOUT:
        		return AUTH_ERR_TIMEOUT;
        	break;
        }


        // --- receive next byte with short polling ---
        int32_t r = uartReceiveData(&ch, 1, 20U);
//        if(ch != '\r')
//        	{
//        	outputLogf("\r\x1b[2K %c",ch);
//        	}
        if (r == UART_ERR_TIMEOUT)
		{
			continue; // no byte this slice
		}
		else if (r == UART_ERR_RECEIVE)
		{
			return AUTH_ERR_FAILURE;
		}

        if(ch == (uint8_t)'\n' )
        {
        	*keylen = len;
        	return AUTH_ERR_OK;
        }

        else if (len < 8u)
        {
        	if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        	{
                key[len++] = ch;
        	}
        }

        else if(len >= 8u)
        {
            return AUTH_ERR_KEY_LENGHT_BREACH;
        }


	}
    return AUTH_ERR_FAILURE;
}

int8_t Auth_ReadAppSignature(void)
{

	const volatile uint8_t* signature_in_flash = (const volatile uint8_t*)0x08010000u;
	sig_copy_in_RAM[0] = signature_in_flash[0];
	sig_copy_in_RAM[1] = signature_in_flash[1];
	sig_copy_in_RAM[2] = signature_in_flash[2];
	sig_copy_in_RAM[3] = signature_in_flash[3];

	return AUTH_ERR_OK;
}


int8_t Auth_Init(void)
{

	schedInitialize(&auth_Scheduler);
	auth_Scheduler.pTask_250ms = Flash_D1;
	ledSetLED(LED0, LED_ON);


	return AUTH_ERR_OK;
}

int8_t Auth_goToFailure(void)
{
	ledSetLED(LED4, LED_ON);

	return AUTH_ERR_OK;
}


/***** PRIVATE FUNCTIONS *****************************************************/


static void switch_to_app(void)
{
	__disable_irq();


	uint32_t *start_app_ptr = (uint32_t *)(APP_STARTHANDLER_ADDR + 4);
	app_start_function start = (app_start_function) *(start_app_ptr);
	start();

	while (1) { }

}



static void Flash_D1(void)
{
    ledToggleLED(LED1);
}




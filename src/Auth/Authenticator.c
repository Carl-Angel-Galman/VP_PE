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

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/



#define WAIT_A_TIMEOUT_MS     (15000u)

#define KEY_STAGE1_MS         (10000u)   // D1 on
#define KEY_STAGE2_MS         (30000u)   // D1 flashing
#define KEY_FAIL_MS           (45000u)

#define KEY_MAX_LEN           (8u)

#define APP_STARTHANDLER_ADDR    (0x08010200)

#define APP_SIGNATURE_ADDR (0x08010000)

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

static void wait_for_start_char(void);


static void D1_FlashUpdate(int32_t nowMs);

static void Flash_D1(void) ;

/***** PRIVATE VARIABLES *****************************************************/

extern int8_t  _sloadauth;
extern int8_t  _sauth;
extern int8_t  _eauth;

static int32_t key_len;

int8_t key[] = "VP2026";

volatile int8_t *sig = (volatile int8_t *)APP_SIGNATURE_ADDR ;


int8_t key_input_buffer[8u];

bool key_received = false;

int32_t ms_counter = 0UL;

static KEY_INPUT_STAGES key_input_stage = INITIAL;

static Scheduler auth_Scheduler;

/***** PUBLIC FUNCTIONS ******************************************************/
__attribute__((section(".auth"), used, noinline))
void verify(void)
{
	__disable_irq();


	uint32_t *start_app_ptr = (uint32_t *)(APP_STARTHANDLER_ADDR + 4);
	app_start_function start = (app_start_function) *(start_app_ptr);
	start();

	while (1) { }
    //switch_to_app();
//    if (sig[0]=='U' && sig[1]=='M' && sig[2]=='M' && sig[3]=='S')
//        {
//        }
}



int8_t copy_and_decrypt_auth_section(uint8_t key[])
{
	if(key == 0)
	{
		return AUTH_ERR_INVALID_PTR;
	}
	uint8_t  *dst = &_sauth;
    const uint8_t  *src = &_sloadauth;
    size_t len = (size_t)(&_eauth - &_sauth);

    key_len = (int32_t)strlen(key);   // <-- add this (or make it const)

    memcpy(dst, src, len);
//
//    for (size_t i = 0; i < len; i++)
//	{
//		dst[i] ^= key[i % key_len];
//	}

    //__DSB();__ISB();

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


int8_t Auth_WaitForA(void)
{
    uint32_t start = HAL_GetTick();
    uint8_t ch;

   // while ((HAL_GetTick() - start) < 15000u)
    //{
        int32_t r = uartReceiveData(&ch, 1, 5000u);

        if (r == UART_ERR_OK)
        {
            if (ch == (uint8_t)'A')
            {
                return AUTH_ERR_OK ;
            }
            // ignore other chars
        }
        else if (r == UART_ERR_TIMEOUT)
        {
            return AUTH_ERR_TIMEOUT; // treat UART error as failure
        }
        // UART_ERR_TIMEOUT -> just keep looping
    //}

    return -1; // 15s timeout
}


int8_t Auth_ReadKey(uint8_t key[8], uint8_t *outLen)
{
    uint32_t start = HAL_GetTick();
    uint32_t now;
    uint32_t elapsed;

    uint8_t len = 0;
    uint8_t ch;

	ledSetLED(LED1, LED_OFF);

    *outLen = 0;

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
        		key_input_stage = FIRST_WARNING;
        	}
        	break;

        case SECOND_WARNING:
        	schedCycle(&auth_Scheduler);
        	if (elapsed >= 45000u)
        	        		key_input_stage = FIRST_WARNING;

        	break;

        case TIMEOUT:
        		return AUTH_ERR_TIMEOUT;
        	break;

        }


        // --- receive next byte with short polling ---
        int32_t r = uartReceiveData(&ch, 1, 20U);

        if (r == UART_ERR_TIMEOUT)
        {
            continue; // no byte this slice
        }
        if (r == UART_ERR_RECEIVE)
        {
            return AUTH_ERR_FAILURE;
        }

        // r == UART_ERR_OK:
        if (ch == (uint8_t)'\n')
        {
            *outLen = len;
            return AUTH_ERR_OK; // key complete
        }

        // max 8 bytes
        if (len < 8u)
        {
            key[len++] = ch;
        }
        else
        {
            // too long before '\n' -> failure (strict max length)
            return -1;
        }
    }
}

int8_t Auth_init(void)
{
	if(schedInitialize(&auth_Scheduler) != SCHED_ERR_OK)
	{
		return AUTH_ERR_FAILURE;
	}


	return AUTH_ERR_OK;
}

static void D1_FlashUpdate(int32_t nowMs)
{
    static uint32_t lastToggleMs = 0;
    if ((nowMs - lastToggleMs) >= 250u) { // 4 Hz
        lastToggleMs = nowMs;
        ledToggleLED(LED1);
    }
}

static void Flash_D1(void)
{
    ledToggleLED(LED1);

}



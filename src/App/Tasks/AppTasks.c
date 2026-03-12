/******************************************************************************
 * @file AppTasks.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com)
 * @date   03.01.2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************************
 * @brief Implementation of the periodic application tasks.
 *
 * @details
 * This file contains the implementation of application tasks executed by the
 * system scheduler. Each task is triggered periodically with a predefined
 * interval (1 ms, 10 ms, 50 ms, 250 ms). The tasks handle:
 *
 * - Display updates
 * - Input polling (buttons)
 * - Sensor updates
 * - Application state machine execution
 * - Stack monitoring and diagnostics
 *
 ******************************************************************************/

/***** INCLUDES **************************************************************/
#include "Scheduler.h"
#include "AppTasks.h"
#include "Application.h"
#include "StackMonitor.h"
#include "HMI/DisplayHandler.h"
#include "stdbool.h"
#include "AppContext.h"
#include "Log/LogOutput.h"

/***** PRIVATE CONSTANTS *****************************************************/

/**
 * @brief Enables the use of a custom Main Stack Pointer (MSP)
 *
 * @details
 * When enabled, the stack pointer is temporarily modified to allow
 * advanced stack scanning and monitoring operations.
 */
#define USE_CUSTOM_MSP 0

/***** PRIVATE TYPES *********************************************************/

/***** PRIVATE PROTOTYPES ****************************************************/

/***** PRIVATE VARIABLES *****************************************************/

/***** PUBLIC FUNCTIONS ******************************************************/

/**
 * @brief 1 ms application task
 *
 * @details
 * This task is executed every 1 millisecond by the scheduler.
 * It updates the two-digit display by multiplexing the display segments.
 *
 * @note
 * The high refresh rate ensures flicker-free display operation.
 */
void taskApp1ms(void)
{
    DisplayHandlerDisplayTwoDigits();
}


/**
 * @brief 10 ms application task
 *
 * @details
 * This task performs periodic polling of input devices and sensors.
 * If a valid event is detected, it is forwarded to the application
 * event handling system.
 *
 * The following components are processed:
 * - Button input polling
 * - Sensor value updates
 *
 * @note
 * Events are forwarded using the AppSendEvent() function.
 */
void taskApp10ms(void)
{
    /** Button polling result */
    int32_t ButtonPollResult = AppPollForButtonEvent();

    /* Send button event if detected */
    if (ButtonPollResult)
    {
        AppSendEvent(ButtonPollResult);
    }

    /** Sensor polling result */
    int32_t SensorPollResult = AppUpdatingSensors();

    /* Send sensor event if detected */
    if(SensorPollResult != NO_EVT)
    {
        AppSendEvent(SensorPollResult);
    }
}


/**
 * @brief 50 ms application task
 *
 * @details
 * Executes the main application logic or state machine.
 * The application run function processes queued events and
 * performs the corresponding actions.
 *
 * If the application run function returns an error, an
 * error event is generated and sent to the event system.
 *
 * @retval none
 */
void taskApp50ms(void)
{
    /** Result of application execution */
    int32_t AppRunError = AppRun();

    /* Send error event if application execution failed */
    if(AppRunError == APP_RUN_ERR)
    {
        AppSendEvent(EVT_ID_ERROR);
    }
}


/**
 * @brief 250 ms application task
 *
 * @details
 * Performs system diagnostics related to stack monitoring.
 * The following checks are executed:
 *
 * - Stack usage measurement
 * - Stack corruption detection
 * - Logging of stack statistics
 *
 * @note
 * This task helps detect stack overflows and memory corruption
 * during runtime.
 */
void taskApp250ms(void)
{

#if defined(USE_CUSTOM_MSP) && (USE_CUSTOM_MSP == 1)

    /**
     * @details
     * Optional stack pointer relocation used for testing.
     */

    extern uint32_t _estack;   /**< End of stack memory */
    extern uint32_t _sstack;   /**< Start of stack scanning region */

    uintptr_t sstartPointer = (uintptr_t)&_estack; /**< Stack start address */
    uintptr_t stackScanner  = (uintptr_t)&_sstack;

    stackScanner--; // One adress lower to go out of stack.

    /** Disable interrupts before modifying stack pointer */
    __disable_irq();

    /** Set new Main Stack Pointer */
    __set_MSP((uint32_t)stackScanner);

    /** Ensure memory operations complete */
    __DSB();
    __ISB();

    /** Re-enable interrupts */
    __enable_irq();

#endif

    /** Get stack statistics */
    uint32_t freeBytes = GetFreeBytes();
    uint32_t usedBytes = GetUsedBytes();
    uint8_t usage = GetUsage();

    /** Log stack usage information */
    outputLogf("free bytes: %d, used bytes: %d, usage: %d \r\n",
               freeBytes, usedBytes, usage);

    /** Check whether the stack is corrupted */
    bool stackCorrupted = isCorrupted();

    /** Send event if stack corruption is detected */
    if(stackCorrupted)
    {
        AppSendEvent(EVT_ID_STACK_CORRUPTION);
    }
}


/***** PRIVATE FUNCTIONS *****************************************************/

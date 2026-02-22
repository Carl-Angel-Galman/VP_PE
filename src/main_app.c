
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
 * @brief Main file for the VP Template project
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

#include "GlobalObjects.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/
static int32_t initializePeripherals();

static void change_vector_table(void);
/***** PRIVATE VARIABLES *****************************************************/
static Scheduler gScheduler;            // Global Scheduler instance


/***** PUBLIC FUNCTIONS ******************************************************/


/**
 * @brief Main function of System
 */
int main(void)
{
    change_vector_table();

    HAL_Init();

    // Initialize the System Clock
    SystemClock_Config();



    // Initialize Peripherals
    initializePeripherals();

    // Initialize Scheduler
    schedInitialize(&gScheduler);

    int globalCounter = 0;
    uint8_t left = 0;

    while (1)
    {

            // Toggle all LEDs to the their functionality (Toggle frequency depends on HAL_Delay at end of loop)
        ledSetLED(LED0, LED_ON);
        ledSetLED(LED1, LED_ON);


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

static void change_vector_table(void)
{
    const uint32_t app_base = 0x08010200;
    uint32_t app_msp = *(uint32_t *)app_base;

    __disable_irq();

    //HAL_RCC_DeInit();
    HAL_DeInit();

    SCB->VTOR = app_base;
    __DSB(); __ISB();

    __HAL_RCC_AHB1_FORCE_RESET();
    __HAL_RCC_AHB1_RELEASE_RESET();
    __enable_irq();
    // Initialize the HAL
}

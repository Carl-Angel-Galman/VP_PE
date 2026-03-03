/**
 * @file      main_state.c
 * @author    Carl Angel Galman
 * @author 	  Liza Henriette Thöne
 * @date      01.05.2023
 *
 * @brief [description]
 */

/*******************************************************************************
* Includes
*******************************************************************************/

#include "StackMonitor.h"


#include "stdbool.h"

#include "Util/Global.h"

/*******************************************************************************
* Defines
*******************************************************************************/

extern uint32_t _estack;

extern uint32_t _sstack;

#define ENDMARKER 0xEA1DADAB

#define MARKER 0xdec0adde

#define STACK_SIZE 0x1000


/*******************************************************************************
* Local Types and Typedefs
*******************************************************************************/

/*******************************************************************************
* Global Variables
*******************************************************************************/


/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/

/*******************************************************************************
* Static Variables
*******************************************************************************/

/*******************************************************************************
* static Functions
*******************************************************************************/

static inline uint32_t get_msp(void)
{
    uint32_t msp;
    __asm volatile ("mrs %0, msp" : "=r"(msp));
    return msp;
}


/*******************************************************************************
* Exported Functions
*******************************************************************************/

/*<
 *
 * @brief retrieves the value of free Bytes of in the Stack by iterating through the stack
 *
 * The Function interates over the stack by using the symbols
 * _sstack and _estack defined in the linker script.
 * The Loop starts at the bottom and runs until the top of the stack
 * or an overwritten memory space.
 *
 * @return the ammount of free bytes counted in the loop
 */
uint32_t GetFreeBytes(void)
{
    uint32_t *p  = &_sstack;
    uint32_t *high = &_estack;

    uint32_t byte_counter = 0;

    while (p < high && *p == MARKER) {
        p++;
        byte_counter++;
    }

    return byte_counter;
}


/*<
 *
 * @brief retrieves the value of used Bytes of in the Stack by iterating through the stack
 *
 * * The Function interates over the stack by using the symbols
 * _sstack and _estack defined in the linker script.
 * The Loop starts at the bottom and runs until the top of the stack
 * or an overwritten memory space, counting the free bytes in the stack,
 * and substracting these from the stack size in bytes
 *
 * @returns the used bytes as the complement of Free Bytes and Stack Size.
 *
 */
uint32_t GetUsedBytes(void)
{
    uint32_t *p  = &_sstack;
    uint32_t *high = &_estack;

    uint32_t byte_counter = 0;

    while (p < high && *p == MARKER) {
        p++;
        byte_counter++;
}

      return STACK_SIZE - byte_counter;

}

uint8_t GetUsage(void)
{

    return GetUsedBytes() >> 12;
}

/*<
 *
 * @brief checks if the bottom of the stack has been overwritten, therefore corrupted
 *
 * @returns bool if the stack is corrupted
 *
 */
bool isCorrupted(void)
{
    uint32_t* p = &_estack;
    if(*p != ENDMARKER)	return true;

    uint32_t sp = get_msp();

    if (sp < (uint32_t)&_sstack || sp > (uint32_t)&_estack) return true;

    return false;

}

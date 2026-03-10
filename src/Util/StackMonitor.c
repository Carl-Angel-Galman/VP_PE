/**
 * @file    StackMonitor.c
 * @author  Carl Angel Galman
 * @author  Liza Henriette Thöne
 * @date    01.05.2023
 *
 * @brief   Functions for monitoring stack usage and detecting stack corruption.
 *
 * This module provides helper functions to:
 * - determine the amount of free stack space
 * - determine the amount of used stack space
 * - calculate the stack usage in percent
 * - detect whether the stack boundaries have been corrupted
 *
 * The implementation assumes that the stack memory is pre-filled with a known
 * marker value during system initialization. As long as a memory word still
 * contains this marker, it is considered unused stack space.
 *
 * The linker symbols `_sstack` and `_estack` define the lower and upper
 * boundaries of the stack region.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "StackMonitor.h"
#include <stdbool.h>
#include <stdint.h>
#include "Util/Global.h"


/*******************************************************************************
 * Defines
 ******************************************************************************/

/**
 * @brief Marker placed at the stack boundary to detect corruption.
 *
 * This value is expected at the configured stack boundary. If it has been
 * overwritten, the stack is assumed to be corrupted.
 */
#define ENDMARKER   0xEA1DADABu

/**
 * @brief Fill pattern used to initialize unused stack memory.
 *
 * During startup, the unused stack area should be filled with this value.
 * Later, the monitor scans the stack region and counts how many words still
 * contain this marker.
 */
#define MARKER      0xDEC0ADDEu

/**
 * @brief Total size of the monitored stack region in bytes.
 *
 * The value is calculated from linker symbols provided by the linker script.
 */
#define STACK_SIZE_BYTES ((uint32_t)((uintptr_t)&_estack - (uintptr_t)&_sstack))

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/**
 * @brief Linker symbol marking the upper boundary of the stack.
 */
extern uint32_t _estack;

/**
 * @brief Linker symbol marking the lower boundary of the stack.
 */
extern uint32_t _sstack;

/*******************************************************************************
 * Static Functions
 ******************************************************************************/

/**
 * @brief Returns the current Main Stack Pointer (MSP) value.
 *
 * This helper reads the current MSP register directly using inline assembly.
 *
 * @return Current MSP value.
 */
static inline uint32_t get_msp(void)
{
    uint32_t msp;
    __asm volatile ("mrs %0, msp" : "=r"(msp));
    return msp;
}

/*******************************************************************************
 * Exported Functions
 ******************************************************************************/

/**
 * @brief Calculates the currently unused stack space in bytes.
 *
 * The function scans the stack memory from the lower stack boundary
 * (`_sstack`) upwards until it finds a value different from `MARKER`.
 *
 * Every word that still contains `MARKER` is treated as unused stack memory.
 * The function returns the corresponding size in bytes.
 *
 * @return Number of free stack bytes.
 */
uint32_t GetFreeBytes(void)
{
    uint32_t *stack_scan = &_sstack + 4;
    uint32_t *stack_top  = &_estack;

    while ((stack_scan < stack_top) && (*stack_scan == MARKER)) {
        stack_scan++;
    }

    return (uint32_t)((uintptr_t)stack_scan - (uintptr_t)&_sstack);
}

/**
 * @brief Calculates the currently used stack space in bytes.
 *
 * The used stack space is determined as:
 *
 * used stack = total stack size - free stack size
 *
 * A safety check ensures that the returned value never exceeds the configured
 * stack size.
 *
 * @return Number of used stack bytes.
 */
uint32_t GetUsedBytes(void)
{
    uint32_t free_bytes = GetFreeBytes();

    if (free_bytes > STACK_SIZE_BYTES) {
        return STACK_SIZE_BYTES;
    }

    return STACK_SIZE_BYTES - free_bytes;
}

/**
 * @brief Returns the stack usage as a percentage.
 *
 * The function calculates the percentage of used stack memory relative to the
 * total stack size.
 *
 * @return Stack usage in percent (0 to 100).
 */
uint8_t GetUsage(void)
{
    return (uint8_t)((GetUsedBytes() * 100u) / STACK_SIZE_BYTES);
}

/**
 * @brief Checks whether the stack is corrupted.
 *
 * The function performs two checks:
 *
 * 1. Verifies that the stack boundary marker (`ENDMARKER`) is still present.
 * 2. Verifies that the current Main Stack Pointer (MSP) lies within the
 *    expected stack boundaries.
 *
 * If either check fails, the stack is considered corrupted.
 *
 * @retVal `true` if stack corruption is detected,
 * @retVal `false`otherwise.
 */
bool isCorrupted(void)
{
    uint32_t *stack_end_marker = &_sstack;

    if (*stack_end_marker != ENDMARKER) {
        return true;
    }

    uint32_t sp = get_msp();

    if ((sp < (uint32_t)&_sstack) || (sp > (uint32_t)&_estack)) {
        return true;
    }

    return false;
}

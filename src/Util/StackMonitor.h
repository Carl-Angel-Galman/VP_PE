/**
 * @file    StackMonitor.h
 * @author  Carl Angel Galman
 * @author  Liza Henriette Thöne
 * @date    01.05.2023
 *
 * @brief   Interface for monitoring stack usage and detecting stack corruption.
 *
 * This module provides function declarations to:
 * - determine the amount of free stack space
 * - determine the amount of used stack space
 * - calculate the stack usage in percent
 * - detect whether the stack boundaries have been corrupted
 */

#ifndef SRC_STACKMONITOR_H_
#define SRC_STACKMONITOR_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdbool.h>
#include "stm32g4xx_hal.h"
#include "Application.h"

/*******************************************************************************
 * Exported Defines
 ******************************************************************************/

/*******************************************************************************
 * Exported Types and Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Exported Functions
 ******************************************************************************/

/**
 * @brief Returns the currently unused stack space in bytes.
 *
 * The function determines how many bytes of the monitored stack region
 * are still unused.
 *
 * @return Number of free stack bytes.
 */
uint32_t GetFreeBytes(void);

/**
 * @brief Returns the currently used stack space in bytes.
 *
 * The function determines how many bytes of the monitored stack region
 * are currently in use.
 *
 * @return Number of used stack bytes.
 */
uint32_t GetUsedBytes(void);

/**
 * @brief Returns the stack usage as a percentage.
 *
 * The function calculates the percentage of used stack memory relative
 * to the total monitored stack size.
 *
 * @return Stack usage in percent.
 */
uint8_t GetUsage(void);

/**
 * @brief Checks whether the stack is corrupted.
 *
 * The function determines whether the configured stack boundary has been
 * exceeded or corrupted.
 *
 * @retval true  Stack corruption detected.
 * @retval false Stack is valid.
 */
bool isCorrupted(void);

#endif /* SRC_STACKMONITOR_H_ */

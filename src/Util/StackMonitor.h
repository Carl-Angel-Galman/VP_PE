/**
 * @file      main_state.c
 * @author    Carl Angel Galman
 * @author 	  Liza Henriette Thöne
 * @date      01.05.2023
 *
 * @brief [description]
 */

#ifndef SRC_STACKMONITOR_H_
#define SRC_STACKMONITOR_H_

/*******************************************************************************
* Includes
*********************************************ch**********************************/


#include "stm32g4xx_hal.h"

#include "Util/Global.h"

#include "stdbool.h"

/*******************************************************************************
* Exported Defines
*******************************************************************************/

/*******************************************************************************
* Exported Types and Typedefs
*******************************************************************************/

/**<
 *  @brief a function that returns free memory space in the stack in Bytes
 *
 *  @return uint32_t free spaces in stack in bytes
 */
uint32_t GetFreeBytes(void);

/**<
 *  @brief a function that returns used memory space in the stack in Bytes
 *
 *  @return uint32_t used spaces in stack in bytes
 */
uint32_t GetUsedBytes(void);

/**<
 *  @brief a function that returns used memory space in percentage in relation to total stack memory space
 *
 *  @return uint8_t returns the percentage value of used memory space in the stack
 */
uint8_t GetUsage(void);

/**<
 *  @brief a function that detemines if the stack is corrupted
 *
 *	The functions reads if the bottom of the stack is reaced and is therefore corrupted.
 *
 */
bool isCorrupted(void);




#endif /* SRC_STACKMONITOR_H_ */

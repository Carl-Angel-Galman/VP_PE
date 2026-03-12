/**
 * @file    DualChannelGas.h
 * @author  Carl Angel Galman
 * @author  Liza Henriette Thöne
 * @date    24.02.2026
 *
 * @brief   Interface for handling a dual-channel gas sensor.
 *
 * This module provides function declarations to:
 * - initialize two gas sensor channels
 * - read ADC voltages for both channels
 * - check both sensor readings for inconsistency
 * - calculate the average gas concentration
 *
 * The module assumes that two redundant gas sensors measure the same
 * physical quantity. Their readings are therefore expected to remain
 * within a defined tolerance.
 */

#ifndef SRC_SERVICE_DUALCHANNELGAS_H_
#define SRC_SERVICE_DUALCHANNELGAS_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include "stm32g4xx_hal.h"

/*******************************************************************************
 * Exported Defines
 ******************************************************************************/

/**
 * @brief Operation completed successfully.
 */
#define DUALSENSORS_OK 0

/**
 * @brief General failure in the dual sensor module.
 */
#define DUALSENSORS_NOT_OK -10

/**
 * @brief Indicates that both sensors are inconsistent beyond the allowed limit.
 */
#define DUALSENSORS_DEFECT -11

/**
 * @brief Indicates that an invalid pointer was passed to a function.
 */
#define DUALSENSORS_INVALID_PTR -12

/*******************************************************************************
 * Exported Functions
 ******************************************************************************/

/**
 * @brief Initializes both gas sensor channels and required peripherals.
 *
 * The function initializes both sensor instances and prepares the ADC
 * interface used to read the sensor voltages.
 *
 * @return `DUALSENSORS_OK` if initialization was successful.
 * @return `DUALSENSORS_NOT_OK` if initialization failed.
 */
int32_t dualGasInit(void);

/**
 * @brief Reads the ADC inputs and updates both gas sensor voltages.
 *
 * The function reads the raw ADC values from the configured ADC channels
 * and forwards them to the corresponding gas sensor instances.
 *
 * @return `DUALSENSORS_OK` if both sensor voltages were updated successfully.
 * @return Error code if one of the sensor updates failed.
 */
int32_t dualGasSetVoltages(void);

/**
 * @brief Checks both gas sensor readings for inconsistency.
 *
 * The function compares the measured gas concentrations of both sensors.
 * If the difference between both values exceeds the allowed percentage
 * threshold, the sensors are considered inconsistent.
 *
 * @return `DUALSENSORS_OK` if both sensors are consistent.
 * @return `DUALSENSORS_DEFECT` if the inconsistency threshold is exceeded.
 * @return `DUALSENSORS_NOT_OK` if sensor values are invalid.
 */
int32_t dualGasCheckInconsistency(void);

/**
 * @brief Calculates the average gas concentration from both sensors.
 *
 * The function retrieves the current values of both gas sensors and
 * calculates their arithmetic mean.
 *
 * @param[out] average Pointer to the variable where the average value
 *                     will be stored.
 *
 * @return `DUALSENSORS_OK` if the average was calculated successfully.
 * @return `DUALSENSORS_INVALID_PTR` if the pointer is NULL.
 * @return `DUALSENSORS_NOT_OK` if sensor values are invalid.
 */
int32_t dualGasGetAverage(int32_t *average);

#endif /* SRC_SERVICE_DUALCHANNELGAS_H_ */

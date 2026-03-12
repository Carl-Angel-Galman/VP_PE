/**
 * @file    GasSensor.h
 * @author  Carl Angel Galman
 * @author  Liza Henriette Thöne
 * @date    10.02.2026
 *
 * @brief   Interface for handling a single gas sensor channel.
 *
 * This module provides function declarations to:
 * - initialize a gas sensor instance
 * - update the measured sensor voltage
 * - convert the measured voltage into a gas concentration value
 *
 * The sensor voltage is expected to be provided in microvolts and is
 * converted to a gas concentration value in ppm using a configurable
 * conversion factor.
 */

#ifndef SRC_SERVICE_GASSENSOR_H_
#define SRC_SERVICE_GASSENSOR_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include "Filter/Filter.h"

/*******************************************************************************
 * Exported Defines
 ******************************************************************************/

/**
 * @brief Operation completed successfully.
 */
#define SENSOR_OK 0

/**
 * @brief Invalid pointer passed to a function.
 */
#define SENSOR_INVALID_PTR -1

/**
 * @brief Invalid conversion factor provided.
 */
#define SENSOR_INVALID_CONVFACTOR -2

/**
 * @brief Calculated sensor value is outside the valid range.
 */
#define SENSOR_INVALID_VALUE -3

/**
 * @brief Sensor voltage is outside the valid operating range.
 */
#define SENSOR_DEFECT -5

/*******************************************************************************
 * Exported Types and Typedefs
 ******************************************************************************/

/**
 * @brief Structure representing a gas sensor instance.
 */
typedef struct _GasSensor
{
	/**
	 * @brief Filtered sensor voltage in microvolts.
	 */
	int32_t sensorVoltage;

	/**
	 * @brief Conversion factor used to convert voltage to ppm.
	 *
	 * Unit: microvolts per ppm.
	 */
	int32_t conversionFactor;

} GasSensor;

/*******************************************************************************
 * Exported Functions
 ******************************************************************************/

/**
 * @brief Initializes a gas sensor instance.
 *
 * The function resets the stored sensor voltage and assigns the given
 * conversion factor used for voltage-to-ppm conversion.
 *
 * @param[in,out] pSensor     Pointer to the gas sensor instance.
 * @param[in]     convFactor  Conversion factor in microvolts per ppm.
 *
 * @return `SENSOR_OK` if initialization was successful.
 * @return `SENSOR_INVALID_PTR` if `pSensor` is NULL.
 * @return `SENSOR_INVALID_CONVFACTOR` if `convFactor` is zero.
 */
int32_t gasSensorInitalize(GasSensor* pSensor, uint32_t convFactor);

/**
 * @brief Updates the stored sensor voltage.
 *
 * The function validates the provided sensor voltage and applies
 * filtering before storing the value in the sensor instance.
 *
 * @param[in,out] pSensor     Pointer to the gas sensor instance.
 * @param[in,out] filter      Pointer to the EMA filter instance.
 * @param[in]     sensorVolt  Sensor voltage in microvolts.
 *
 * @return `SENSOR_OK` if the voltage was updated successfully.
 * @return `SENSOR_INVALID_PTR` if `pSensor` is NULL.
 * @return `SENSOR_DEFECT` if the sensor voltage lies outside the
 *         valid measurement range.
 */
int32_t gasSensorSetSensorVoltage(GasSensor* pSensor, EMAFilterData_t* filter, uint32_t sensorVolt);

/**
 * @brief Returns the gas concentration measured by the sensor.
 *
 * The function converts the stored sensor voltage into a gas
 * concentration value in ppm.
 *
 * @param[in]  pSensor Pointer to the gas sensor instance.
 * @param[out] pValue  Pointer to the variable receiving the gas
 *                     concentration in ppm.
 *
 * @return `SENSOR_OK` if the value was calculated successfully.
 * @return `SENSOR_INVALID_PTR` if one of the pointers is NULL.
 * @return `SENSOR_INVALID_VALUE` if the calculated value lies
 *         outside the valid range.
 * @return `SENSOR_DEFECT` if the stored voltage is invalid.
 */
int32_t gasSensorGetSensorValue(GasSensor* pSensor, int32_t* pValue);

#endif /* SRC_SERVICE_GASSENSOR_H_ */

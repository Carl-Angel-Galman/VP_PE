/**
 * @file    GasSensor.c
 * @author  Carl Angel Galman
 * @author  Liza Henriette Thöne
 * @date    10.02.2026
 *
 * @brief   Functions for handling a single gas sensor channel.
 *
 * This module provides helper functions to:
 * - initialize a gas sensor instance
 * - set and filter the measured sensor voltage
 * - convert the sensor voltage into a gas concentration value
 *
 * The implementation assumes that the gas sensor output voltage is within
 * a defined valid range. The measured voltage is filtered before being stored
 * in the sensor instance.
 *
 * The gas concentration is calculated from the filtered sensor voltage using
 * a linear conversion model with a fixed voltage offset and a configurable
 * conversion factor.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stddef.h>
#include "GasSensor.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/**
 * @brief Minimum valid gas concentration value in ppm.
 *
 * Values below this threshold are treated as invalid.
 */
#define MIN_SENOSR_VALUE 200

/**
 * @brief Maximum valid gas concentration value in ppm.
 *
 * Values above this threshold are treated as invalid.
 */
#define MAX_SENOSR_VALUE 10000

/**
 * @brief Minimum valid sensor voltage in microvolts.
 */
#define MIN_VOLT_VALUE 500000

/**
 * @brief Maximum valid sensor voltage in microvolts.
 */
#define MAX_VOLT_VALUE 2500000

/**
 * @brief Voltage offset used for concentration calculation in microvolts.
 *
 * This offset represents the lower baseline of the sensor characteristic.
 */
#define VOLT_OFFSET 500000

/*******************************************************************************
 * Exported Functions
 ******************************************************************************/

/**
 * @brief Initializes a gas sensor instance.
 *
 * The function resets the stored sensor voltage and assigns the provided
 * conversion factor to the sensor instance.
 *
 * A valid conversion factor must be non-zero. A null pointer is rejected.
 *
 * @param[in,out] pSensor     Pointer to the gas sensor instance to initialize.
 * @param[in]     convFactor  Conversion factor used for voltage-to-ppm
 *                            calculation.
 *
 * @return `SENSOR_OK` if initialization was successful.
 * @return `SENSOR_INVALID_PTR` if `pSensor` is `NULL`.
 * @return `SENSOR_INVALID_CONVFACTOR` if `convFactor` is zero.
 */
int32_t gasSensorInitalize(GasSensor* pSensor, uint32_t convFactor)
{
	if(pSensor == NULL)
	{
		return SENSOR_INVALID_PTR;
	}

	pSensor->sensorVoltage = 0;
	if(convFactor == 0)
	{
		return SENSOR_INVALID_CONVFACTOR;
	}
	pSensor->conversionFactor = convFactor;

	return SENSOR_OK;
}

/**
 * @brief Updates the sensor voltage after validating and filtering it.
 *
 * The function checks whether the raw sensor voltage lies within the valid
 * measurement range. If valid, the voltage is processed using the supplied
 * EMA filter and stored in the sensor instance.
 *
 * Validation of the sensor pointer is performed before further processing.
 *
 * @param[in,out] pSensor     Pointer to the gas sensor instance.
 * @param[in,out] filter      Pointer to the EMA filter instance.
 * @param[in]     sensorVolt  Raw sensor voltage in microvolts.
 *
 * @return `SENSOR_OK` if the voltage was processed successfully.
 * @return `SENSOR_INVALID_PTR` if `pSensor` is `NULL`.
 * @return `SENSOR_DEFECT` if `sensorVolt` lies outside the valid voltage range.
 * @return Error code returned by `filterEMA()` if filtering failed.
 */
int32_t gasSensorSetSensorVoltage(GasSensor* pSensor,EMAFilterData_t* filter, uint32_t sensorVolt)
{
	if(pSensor == NULL)
	{
		return SENSOR_INVALID_PTR;
	}
	if(sensorVolt < MIN_VOLT_VALUE|| sensorVolt > MAX_VOLT_VALUE )
	{
		return SENSOR_DEFECT;
	}
	int32_t checkFilter = filterEMA(filter, sensorVolt,&pSensor->sensorVoltage );
	if(checkFilter != FILTER_ERR_OK)
	{
		return checkFilter;
	}

	return SENSOR_OK;
}

/**
 * @brief Converts the stored sensor voltage into a gas concentration value.
 *
 * The function validates the stored sensor voltage and then calculates the
 * gas concentration in ppm using the following linear model:
 *
 * `value = MIN_SENOSR_VALUE + ((sensorVoltage - VOLT_OFFSET) / conversionFactor)`
 *
 * The result is validated against the configured concentration limits before
 * being written to the output pointer.
 *
 * @param[in]  pSensor  Pointer to the gas sensor instance.
 * @param[out] pValue   Pointer to the variable that receives the calculated
 *                      gas concentration in ppm.
 *
 * @return `SENSOR_OK` if the gas concentration was calculated successfully.
 * @return `SENSOR_INVALID_PTR` if `pSensor` or `pValue` is `NULL`.
 * @return `SENSOR_DEFECT` if the stored sensor voltage lies outside the valid
 *         voltage range.
 * @return `SENSOR_INVALID_VALUE` if the calculated gas concentration lies
 *         outside the valid concentration range.
 */
int32_t gasSensorGetSensorValue(GasSensor* pSensor, int32_t* pValue)
{
	if(pSensor == NULL)
	{
		return SENSOR_INVALID_PTR;
	}
	if(pValue == NULL)
	{
		return SENSOR_INVALID_PTR;
	}

	if((pSensor->sensorVoltage < MIN_VOLT_VALUE )|| (pSensor->sensorVoltage > MAX_VOLT_VALUE))
	{
		return SENSOR_DEFECT;
	}
	//Difference from sensorVoltage and Offset
	int32_t deltaVoltage = pSensor->sensorVoltage - VOLT_OFFSET;

	int32_t value = MIN_SENOSR_VALUE + (int32_t)(deltaVoltage/pSensor->conversionFactor);

	//Validate the sensor value
	if(value < MIN_SENOSR_VALUE || value > MAX_SENOSR_VALUE)
		return SENSOR_INVALID_VALUE;
	*pValue =value;

	return SENSOR_OK;
}

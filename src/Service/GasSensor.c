/*
 * GasSensor.c
 *
 *  Created on: Feb 10, 2026
 *      Author: kali
 */

#include <stddef.h>
#include "GasSensor.h"

#define MIN_SENOSR_VALUE 200
#define MAX_SENOSR_VALUE 10000
#define MIN_VOLT_VALUE 500000
#define MAX_VOLT_VALUE 2500000
#define VOLT_OFFSET 500000

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

int32_t gasSensorSetSensorVoltage(GasSensor* pSensor, uint32_t sensorVolt)
{
	if(pSensor == NULL)
		{
			return SENSOR_INVALID_PTR;
		}
	if(sensorVolt < MIN_VOLT_VALUE|| sensorVolt > MAX_VOLT_VALUE )
		{
			return SENSOR_DEFECT;
		}
	pSensor->sensorVoltage = sensorVolt;

	return SENSOR_OK;


}

int32_t gasSensorGetSensorValue(GasSensor* pSensor)
{
	if(pSensor == NULL)
			{
				return SENSOR_INVALID_PTR;
			}

	if(pSensor->sensorVoltage < MIN_VOLT_VALUE|| pSensor->sensorVoltage > MAX_VOLT_VALUE )
			{
				return SENSOR_DEFECT;
			}
	//Difference from sensorVoltage and Offset
	int32_t deltaVoltage = pSensor->sensorVoltage - VOLT_OFFSET;

	int32_t value = MIN_SENOSR_VALUE + (int32_t)(deltaVoltage/pSensor->conversionFactor);

	//Validate the sensor value
	if(value < MIN_SENOSR_VALUE || value > MAX_SENOSR_VALUE)
		return SENSOR_INVALID_VALUE;

	return value;
}

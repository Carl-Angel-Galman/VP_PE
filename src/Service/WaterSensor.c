/*
 * WaterSensor.c
 *
 *  Created on: Mar 3, 2026
 *      Author: kali
 */
#include <stddef.h>
#include "WaterSensor.h"

#define MIN_SENOSR_VALUE 50	//in cm
#define MAX_SENOSR_VALUE 1000//in cm
#define MIN_VOLT_VALUE 500000 //in microVolt
#define MAX_VOLT_VALUE 2500000
#define VOLT_OFFSET 500000

int32_t waterSensorInitalize(WaterSensor* pSensor, uint32_t convFactor)
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

int32_t waterSensorSetSensorVoltage(WaterSensor* pSensor, uint32_t sensorVolt)
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

int32_t waterSensorGetSensorValue(WaterSensor* pSensor)
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

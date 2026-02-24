/*
 * GasSensor.c
 *
 *  Created on: Feb 18, 2026
 *      Author: kali
 */

#include <stddef.h>
#include "GasSensor.h"

#define MIN_SENOSR_VALUE 200
#define MAX_SENOSR_VALUE 10000

int32_t gasSensorInitalize(GasSensor* pSensor, uint32_t convFactor)
{

	if(pSensor == NULL)
	{
		return SENOSR_INVALID_PTR;
	}

	pSensor->sensorVoltage = 0;
	pSensor->conversionFactor = convFactor;

	return SENSOR_OK;
}

int32_t gasSensorSetSensorVoltage(GasSensor* pSensor, uint32_t sensorVolt)
{
	if(pSensor == NULL)
		{
			return SENOSR_INVALID_PTR;
		}
	pSensor->sensorVoltage = sensorVolt;

	return SENSOR_OK;


}

int32_t gasSensorGetSensorValue(GasSensor* pSensor)
{
	if(pSensor == NULL)
			{
				return SENOSR_INVALID_PTR;
			}
	//Fehler vom Sensor noch beachten und Offset mit in die Berechnung einfügen

	int32_t value = (int32_t)(pSensor->sensorVoltage/pSensor->conversionFactor);
	if(value <0 || value >1000) //hier MagicNumber bitte vermeiden, define oben vrwenden
		return SENSOR_INVALID_VALUE;

	return value;
}




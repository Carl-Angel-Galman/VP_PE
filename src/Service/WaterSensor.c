/*
 * WaterSensor.c
 *
 *  Created on: Mar 3, 2026
 *      Author: kali
 */
#include <stddef.h>
#include "WaterSensor.h"

#define MIN_SENSOR_VALUE 50	//in cm
#define MAX_SENSOR_VALUE 1000//in cm
#define MIN_VOLT_VALUE 500000 //in microVolt
#define MAX_VOLT_VALUE 2500000
#define VOLT_OFFSET 500000
#define CONV_FACTOR 2105 //in microVolt

static WaterSensor wSensor;

int32_t waterSensorInitalize()
{

	wSensor.sensorVoltage = 0;
	wSensor.conversionFactor = CONV_FACTOR;

	return WATER_SENSOR_OK;
}

int32_t waterSensorSetSensorVoltage()
{
	int32_t sensorVolt = readfromPythonScript();

		if(sensorVolt < MIN_VOLT_VALUE|| sensorVolt > MAX_VOLT_VALUE )
			{
				return WATER_SENSOR_DEFECT;
			}
		wSensor.sensorVoltage = sensorVolt;

		return WATER_SENSOR_OK;
}

int32_t waterSensorGetSensorValue()
{
	if(wSensor.sensorVoltage < MIN_VOLT_VALUE ||
	       wSensor.sensorVoltage > MAX_VOLT_VALUE)
	        return WATER_SENSOR_DEFECT;

	    int32_t deltaVoltage = wSensor.sensorVoltage - VOLT_OFFSET;

	    int32_t value = MIN_SENSOR_VALUE +
	                    (deltaVoltage / wSensor.conversionFactor);

	    if(value < MIN_SENSOR_VALUE || value > MAX_SENSOR_VALUE)
	        return WATER_SENSOR_INVALID_VALUE;

	    return value;
}

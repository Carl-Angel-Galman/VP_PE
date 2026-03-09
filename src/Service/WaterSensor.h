/*
 * WaterSensor.h
 *
 *  Created on: Mar 3, 2026
 *      Author: kali
 */

#ifndef SRC_SERVICE_WATERSENSOR_H_
#define SRC_SERVICE_WATERSENSOR_H_

#include <stdint.h>

#define WATER_SENSOR_OK			0
#define SENSOR_INVALID_PTR	-1
#define WATER_SENSOR_INVALID_VALUE -3
#define WATER_SENSOR_DEFECT -5
#define NO_DATA_GET -7

typedef struct _WaterSensor
{
	//sensorVoltage in microVolt
	uint32_t sensorVoltage;

	//Conversion Factor microVolt per ppm
	uint32_t conversionFactor;

} WaterSensor;

//Initalize the WaterSensor, return SENSOR_OK if no error occured
int32_t waterSensorInitalize();

//Input from python Script,  return SENSOR_OK if no error occured
int32_t waterSensorSetSensorVoltage();

//Returns the cm-Value of sensor
int32_t waterSensorGetSensorValue(int32_t* waterlevel);



#endif /* SRC_SERVICE_WATERSENSOR_H_ */

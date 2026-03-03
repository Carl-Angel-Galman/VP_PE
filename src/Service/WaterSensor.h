/*
 * WaterSensor.h
 *
 *  Created on: Mar 3, 2026
 *      Author: kali
 */

#ifndef SRC_SERVICE_WATERSENSOR_H_
#define SRC_SERVICE_WATERSENSOR_H_

#include <stdint.h>

#define SENSOR_OK			0
#define SENSOR_INVALID_PTR	-1
#define SENSOR_INVALID_CONVFACTOR -2
#define SENSOR_INVALID_VALUE -3
#define SENSOR_DEFECT -5
#define NO_DATA_GET -7

typedef struct _WaterSensor
{
	//sensorVoltage in microVolt
	uint32_t sensorVoltage;

	//Conversion Factor microVolt per ppm
	uint32_t conversionFactor;

} WaterSensor;

//Initalize the WaterSensor, return SENSOR_OK if no error occured
int32_t waterSensorInitalize(WaterSensor* pSensor, uint32_t convFactor);

//Input sensorVolt in mircoVoltage,  return SENSOR_OK if no error occured
int32_t waterSensorSetSensorVoltage(WaterSensor* pSensor, uint32_t sensorVolt);

//Returns the cm-Value of sensor
int32_t waterSensorGetSensorValue(WaterSensor* pSensor);



#endif /* SRC_SERVICE_WATERSENSOR_H_ */

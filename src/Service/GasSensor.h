/*
 * GasSensor.h
 *
 *  Created on: Feb 10, 2026
 *      Author: kali
 */

#ifndef SRC_SERVICE_GASSENSOR_H_
#define SRC_SERVICE_GASSENSOR_H_

#include <stdint.h>

#define SENSOR_OK			0
#define SENSOR_INVALID_PTR	-1
#define SENSOR_INVALID_CONVFACTOR -2
#define SENSOR_INVALID_VALUE -3
#define SENSOR_DEFECT -5

typedef struct _GasSensor
{
	//sensorVoltage in microVolt
	uint32_t sensorVoltage;

	//Conversion Factor microVolt per ppm
	uint32_t conversionFactor;

} GasSensor;

//Initalize the gasSensor, return SENSOR_OK if no error occured
int32_t gasSensorInitalize(GasSensor* pSensor, uint32_t convFactor);

//Input sensorVolt in mircoVoltage,  return SENSOR_OK if no error occured
int32_t gasSensorSetSensorVoltage(GasSensor* pSensor, uint32_t sensorVolt);

//Returns the ppm-Value of sensor
int32_t gasSensorGetSensorValue(GasSensor* pSensor);


#endif /* SRC_SERVICE_GASSENSOR_H_ */



/*
 * GasSensor.h
 *
 *  Created on: Feb 18, 2026
 *      Author: kali
 */

#ifndef SRC_SERVICE_GASSENSOR_H_
#define SRC_SERVICE_GASSENSOR_H_


#include <stdint.h>

#define SENSOR_OK			0
#define SENOSR_INVALID_PTR	-1
#define SENSOR_INVALID_VALUE -3

typedef struct GasSensor
{
	//sensorVoltage in microVolt
	uint32_t sensorVoltage;

	//Conversion Factor microVolt per ppm
	uint32_t conversionFactor;

} GasSensor;

int32_t gasSensorInitalize(GasSensor* pSensor, uint32_t convFactor);
int32_t gasSensorSetSensorVoltage(GasSensor* pSensor, uint32_t sensorVolt);

int32_t gasSensorGetSensorValue(GasSensor* pSensor);


#endif /* SRC_SERVICE_GASSENSOR_H_ */



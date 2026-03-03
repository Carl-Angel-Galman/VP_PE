/*
 * DualChannelGas.c
 *
 *  Created on: Feb 24, 2026
 *      Author: kali
 */

#include <stdbool.h>
#include "DualChannelGas.h"
#include "ADCModule.h"
#include "Filter/Filter.h"


#define NUMBERS_OF_SENSORS 2
#define CONVERSION_FACTOR  204 //in microvoltage
#define MIN_SENOSR_VALUE 200 //in ppm
#define MAX_SENOSR_VALUE 10000 //in ppm
#define INCONSISTENCY_VALUE 10 //in percent
#define CHECK_NULL_NEG 0
#define FILTER_ALPHA 400
#define FILTER_SCALING 1000

static GasSensor sensorP1;
static GasSensor sensorP2;
static EMAFilterData_t filter;
static bool resetFilter = false;


int32_t dualGasInit()
{
	//Init ADC for reading the data
	adcInitialize();

	int32_t initCheck1 = gasSensorInitalize(&sensorP1, CONVERSION_FACTOR);
	//Check successful initialization
	if(initCheck1 != SENSOR_OK)
		return SENSORS_NOT_OK;

	int32_t initCheck2 = gasSensorInitalize(&sensorP2, CONVERSION_FACTOR);
	//Check successful initialization
	if(initCheck2 != SENSOR_OK)
			return SENSORS_NOT_OK;
	if(resetFilter == false)
	{
		int32_t filterCheck = filterInitEMA(&filter, FILTER_SCALING, FILTER_ALPHA);
		if(filterCheck != FILTER_ERR_OK)
			return FILTER_ERR_GENERAL;
		resetFilter = true;
	}
	else if(resetFilter == true)
	{
		int32_t filterCheck = filterResetEMA(&filter);
		if(filterCheck != FILTER_ERR_OK)
			return FILTER_ERR_GENERAL;
	}

	return SENSORS_OK;
}

int32_t dualGasSetVoltages()
{
	//Reading data of ADC
	int32_t Voltage1 = adcReadChannel(ADC_INPUT0);
	//No error code possible, validation takes place in the gasSensorSetSensorVoltage
	int32_t checkSetted = gasSensorSetSensorVoltage(&sensorP1, Voltage1);
	if(checkSetted != SENSOR_OK)
	{
		//CheckSetted return the error codes of gasSensorSetSensorVoltage
		return checkSetted;
	}
	int32_t Voltage2 = adcReadChannel(ADC_INPUT1);
	checkSetted = gasSensorSetSensorVoltage(&sensorP2, Voltage2);
	if(checkSetted != SENSOR_OK)
	{
		return checkSetted;
	}

	return SENSORS_OK;
}

int32_t dualGasCheckInconsistency()
{
	int32_t maxValue = 0;
	int32_t ppm_sensor1 = gasSensorGetSensorValue(&sensorP1);
	int32_t ppm_sensor2 = gasSensorGetSensorValue(&sensorP2);

	//Checking for invalid data or error codes
	if(ppm_sensor1 < MIN_SENOSR_VALUE || ppm_sensor2 < MIN_SENOSR_VALUE)
	{
		return SENSORS_NOT_OK;
	}
	int32_t diff = ppm_sensor1 - ppm_sensor2;
	//Check if the difference is negative
	if(diff < 0)
	{
	    diff = -diff;
	    maxValue = ppm_sensor2;
	}
	else
	{
		maxValue = ppm_sensor1;
	}

	//Checking for 10% inconsistency
	if(diff*INCONSISTENCY_VALUE >= maxValue)
	{
		return SENSORS_DEFECT;
	}

	return SENSORS_OK;
}

int32_t dualGasGetAverage()
{
	int32_t ppm_sensor1 = gasSensorGetSensorValue(&sensorP1);
	int32_t ppm_sensor2 = gasSensorGetSensorValue(&sensorP2);

	//Checking for invalid data or error codes
	if(ppm_sensor1 < MIN_SENOSR_VALUE || ppm_sensor2 < MIN_SENOSR_VALUE)
	{
		return SENSORS_NOT_OK;
	}

	//Calculation of the average
	int32_t average = (int32_t) (ppm_sensor1 + ppm_sensor2)/NUMBERS_OF_SENSORS;

	int32_t filteredAverage = filterEMA(&filter, average);

	return filteredAverage;
}

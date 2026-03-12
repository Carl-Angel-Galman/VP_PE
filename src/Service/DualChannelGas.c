/**
 * @file    DualChannelGas.c
 * @author  kali
 * @date    24.02.2026
 *
 * @brief   Functions for handling and validating a dual-channel gas sensor.
 *
 * This module provides helper functions to:
 * - initialize two gas sensor channels
 * - acquire ADC voltages for both channels
 * - update the corresponding gas sensor values
 * - detect inconsistency between both sensor readings
 * - calculate the average gas concentration
 *
 * Each sensor channel has its own EMA filter instance. This is important,
 * because both channels must be filtered independently.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdbool.h>
#include <limits.h>
#include <stdint.h>
#include "DualChannelGas.h"
#include "GasSensor.h"
#include "ADCModule.h"
#include "Filter/Filter.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define NUMBERS_OF_SENSORS       2
#define CONVERSION_FACTOR        204

#define MIN_SENSOR_VALUE         200
#define MAX_SENSOR_VALUE         10000

#define INCONSISTENCY_VALUE      10


#define FILTER_ALPHA             400
#define FILTER_SCALING           1000

#define PERCENTAGE_MULTIPLIER    100

/*******************************************************************************
 * Static Variables
 ******************************************************************************/

static GasSensor sensorP1;
static GasSensor sensorP2;

/* One filter per sensor channel */
static EMAFilterData_t filterP1;
static EMAFilterData_t filterP2;

static bool filtersInitialized = false;

/*******************************************************************************
 * Exported Functions
 ******************************************************************************/

int32_t dualGasInit(void)
{
    int32_t initCheck1 = gasSensorInitalize(&sensorP1, CONVERSION_FACTOR);
    if(initCheck1 != SENSOR_OK)
    {
        return DUALSENSORS_NOT_OK;
    }

    int32_t initCheck2 = gasSensorInitalize(&sensorP2, CONVERSION_FACTOR);
    if(initCheck2 != SENSOR_OK)
    {
        return DUALSENSORS_NOT_OK;
    }

    if(filtersInitialized == false)
    {
        int32_t filterCheck = filterInitEMA(&filterP1, FILTER_SCALING, FILTER_ALPHA);
        if(filterCheck != FILTER_ERR_OK)
        {
            return FILTER_ERR_GENERAL;
        }

        filterCheck = filterInitEMA(&filterP2, FILTER_SCALING, FILTER_ALPHA);
        if(filterCheck != FILTER_ERR_OK)
        {
            return FILTER_ERR_GENERAL;
        }

        filtersInitialized = true;
    }
    else
    {
        int32_t filterCheck = filterResetEMA(&filterP1);
        if(filterCheck != FILTER_ERR_OK)
        {
            return FILTER_ERR_GENERAL;
        }

        filterCheck = filterResetEMA(&filterP2);
        if(filterCheck != FILTER_ERR_OK)
        {
            return FILTER_ERR_GENERAL;
        }
    }

    return DUALSENSORS_OK;
}

int32_t dualGasSetVoltages(void)
{
    int32_t voltage1 = adcReadChannel(ADC_INPUT0);
    int32_t checkSetted = gasSensorSetSensorVoltage(&sensorP1, &filterP1, voltage1);
    if(checkSetted != SENSOR_OK)
    {
        return checkSetted;
    }

    int32_t voltage2 = adcReadChannel(ADC_INPUT1);
    checkSetted = gasSensorSetSensorVoltage(&sensorP2, &filterP2, voltage2);
    if(checkSetted != SENSOR_OK)
    {
        return checkSetted;
    }

    return DUALSENSORS_OK;
}

int32_t dualGasCheckInconsistency(void)
{
    int32_t ppm_sensor1 = 0;
    int32_t ppm_sensor2 = 0;

    int32_t checkSuccess = gasSensorGetSensorValue(&sensorP1, &ppm_sensor1);
    if(checkSuccess != SENSOR_OK)
    {
        return checkSuccess;
    }

    checkSuccess = gasSensorGetSensorValue(&sensorP2, &ppm_sensor2);
    if(checkSuccess != SENSOR_OK)
    {
        return checkSuccess;
    }

    if((ppm_sensor1 < MIN_SENSOR_VALUE) || (ppm_sensor1 > MAX_SENSOR_VALUE) ||
       (ppm_sensor2 < MIN_SENSOR_VALUE) || (ppm_sensor2 > MAX_SENSOR_VALUE))
    {
        return DUALSENSORS_NOT_OK;
    }

    int32_t diff = ppm_sensor1 - ppm_sensor2;
    if(diff < 0)
    {
        diff = -diff;
    }

    int32_t maxValue = (ppm_sensor1 > ppm_sensor2) ? ppm_sensor1 : ppm_sensor2;

    if(maxValue <= 0)
    {
        return DUALSENSORS_NOT_OK;
    }

    if(diff > (INT32_MAX / PERCENTAGE_MULTIPLIER))
    {
        return DUALSENSORS_NOT_OK;
    }

    if(maxValue > (INT32_MAX / INCONSISTENCY_VALUE))
    {
        return DUALSENSORS_NOT_OK;
    }

    if((diff * PERCENTAGE_MULTIPLIER) >= (maxValue * INCONSISTENCY_VALUE))
    {
        return DUALSENSORS_DEFECT;
    }

    return DUALSENSORS_OK;
}

int32_t dualGasGetAverage(int32_t* average)
{
    if(average == NULL)
    {
        return DUALSENSORS_INVALID_PTR;
    }

    int32_t ppm_sensor1 = 0;
    int32_t ppm_sensor2 = 0;

    int32_t checkSuccess = gasSensorGetSensorValue(&sensorP1, &ppm_sensor1);
    if(checkSuccess != SENSOR_OK)
    {
        return checkSuccess;
    }

    checkSuccess = gasSensorGetSensorValue(&sensorP2, &ppm_sensor2);
    if(checkSuccess != SENSOR_OK)
    {
        return checkSuccess;
    }

    if((ppm_sensor1 < MIN_SENSOR_VALUE) || (ppm_sensor1 > MAX_SENSOR_VALUE) ||
       (ppm_sensor2 < MIN_SENSOR_VALUE) || (ppm_sensor2 > MAX_SENSOR_VALUE))
    {
        return DUALSENSORS_NOT_OK;
    }

    /* Overflow-safe average */
    *average = ppm_sensor1 / NUMBERS_OF_SENSORS + ppm_sensor2 / NUMBERS_OF_SENSORS;

    /*Check if both division had a 0.5, then add 1 too average*/
    if((ppm_sensor1 % NUMBERS_OF_SENSORS + ppm_sensor2 % NUMBERS_OF_SENSORS) >= NUMBERS_OF_SENSORS)
    {
        (*average)++;
   }

    return DUALSENSORS_OK;
}

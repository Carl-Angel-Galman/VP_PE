/**
 * @file    WaterSensor.c
 * @author  Carl Angel Galman
 * @author  Liza Henriette Thöne
 * @date    03.03.2026
 *
 * @brief   Functions for handling a water level sensor.
 *
 * This module provides helper functions to:
 * - initialize the water sensor instance
 * - update the measured sensor voltage
 * - convert the sensor voltage into a water level value
 * - receive UART packets of the water sensor
 * - validate CRC and packet counter
 * - supervise packet timeout
 *
 * The water level is calculated from the measured sensor voltage using
 * a linear conversion model with a fixed voltage offset and a defined
 * conversion factor.
 *
 * The sensor voltage must lie within a defined valid range. If the voltage
 * lies outside this range, the sensor is considered defective.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "WaterSensor.h"
#include "UARTModule.h"
#include "stm32g4xx_hal.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/**
 * @brief Minimum valid water level in centimeters.
 */
#define MIN_SENSOR_VALUE 50

/**
 * @brief Maximum valid water level in centimeters.
 */
#define MAX_SENSOR_VALUE 1000

/**
 * @brief Minimum valid sensor voltage in microvolts.
 */
#define MIN_VOLT_VALUE 500000

/**
 * @brief Maximum valid sensor voltage in microvolts.
 */
#define MAX_VOLT_VALUE 2500000

/**
 * @brief Voltage offset used for water level calculation in microvolts.
 */
#define VOLT_OFFSET 500000

/**
 * @brief Conversion factor for voltage-to-level calculation.
 *
 * Unit: microvolts per centimeter.
 */
#define CONV_FACTOR 2105

/**
 * @brief Timeout threshold for missing UART packets in milliseconds.
 */
#define WATER_SENSOR_TIMEOUT_MS 1500U

/**
 * @brief Byte index of the packet counter.
 */
#define WATER_SENSOR_COUNTER_INDEX 0U

/**
 * @brief Byte index of the high byte of the sensor voltage.
 */
#define WATER_SENSOR_VOLTAGE_HIGH_INDEX 1U

/**
 * @brief Byte index of the low byte of the sensor voltage.
 */
#define WATER_SENSOR_VOLTAGE_LOW_INDEX 2U

/**
 * @brief Byte index of the CRC byte.
 */
#define WATER_SENSOR_CRC_INDEX 3U

/**
 * @brief Number of bytes covered by the CRC calculation.
 */
#define WATER_SENSOR_CRC_DATA_LENGTH (WATER_SENSOR_PACKET_SIZE - 1U)

#define WATER_SENSOR_PLACEHOLDER 952315

/*******************************************************************************
 * Static Variables
 ******************************************************************************/
/**
 * @brief Static instance of the water sensor.
 */
static WaterSensor gWaterSensor;

/*******************************************************************************
 * Private Prototypes
 ******************************************************************************/

static uint8_t waterSensorCalculateCrc(const uint8_t *data, uint8_t length);
static int32_t waterSensorValidatePacket(const uint8_t *packet);
static void waterSensorStorePacket(const uint8_t *packet);

/*******************************************************************************
 * Exported Functions
 ******************************************************************************/

/**
 * @brief Initializes the water sensor instance.
 *
 * The function resets the stored sensor voltage and assigns the predefined
 * conversion factor used for voltage-to-water-level conversion.
 *
 * Additionally, the UART packet handling context is reset.
 *
 * @return `WATER_SENSOR_OK` if initialization was successful.
 */
int32_t waterSensorInitalize(void)
{
	gWaterSensor.sensorVoltage = 0;
	gWaterSensor.conversionFactor = CONV_FACTOR;

    (void)memset(&gWaterSensor, 0, sizeof(gWaterSensor));
    gWaterSensor.lastValidPacketTick = HAL_GetTick();
    gWaterSensor.initialized = true;
    gWaterSensor.newValueAvailable = false;

	return WATER_SENSOR_OK;
}

/**
 * @brief Updates the stored sensor voltage.
 *
 * The function reads the sensor voltage and validates whether it lies
 * within the valid operating range. If valid, the value is stored in
 * the sensor instance.
 *
 * @return `WATER_SENSOR_OK` if the voltage was updated successfully.
 * @return `WATER_SENSOR_DEFECT` if the sensor voltage lies outside the
 *         valid voltage range.
 */
int32_t waterSensorSetSensorVoltage(void)
{

    if(gWaterSensor.initialized == false)
    {
        return WATER_SENSOR_ERR;
    }
    /*
    if(gWaterSensor.newValueAvailable == false)
    {
    	if(waterSensorCheckTimeout() != WATER_SENSOR_OK)
    		return WATER_SENSOR_ERR;
        return WATER_SENSOR_OK;
    }*/

	uint32_t sensorVolt = WATER_SENSOR_PLACEHOLDER;

	/*Missing the code to get sensorVoltage from UARTPacket*/

	if(sensorVolt < MIN_VOLT_VALUE || sensorVolt > MAX_VOLT_VALUE)
	{
		return WATER_SENSOR_DEFECT;
	}

	gWaterSensor.sensorVoltage = sensorVolt;

	return WATER_SENSOR_OK;
}

/**
 * @brief Returns the calculated water level.
 *
 * The function converts the stored sensor voltage into a water level
 * value in centimeters using the configured conversion factor.
 *
 * The calculation is based on:
 *
 * `waterlevel = MIN_SENSOR_VALUE + ((sensorVoltage - VOLT_OFFSET) / conversionFactor)`
 *
 * The resulting value is validated against the allowed water level range.
 *
 * @param[out] waterlevel Pointer to the variable that receives the
 *                        calculated water level in centimeters.
 *
 * @return `WATER_SENSOR_OK` if the water level was calculated successfully.
 * @return `WATER_SENSOR_DEFECT` if the stored voltage lies outside the
 *         valid range.
 * @return `WATER_SENSOR_INVALID_VALUE` if the calculated water level lies
 *         outside the allowed range.
 */
int32_t waterSensorGetSensorValue(int32_t* waterlevel)
{
	if(waterlevel == NULL)
	{
		return WATER_SENSOR_INVALID_PTR;
	}
    if(gWaterSensor.initialized == false)
    {
        return WATER_SENSOR_ERR;
    }


	if(gWaterSensor.sensorVoltage < MIN_VOLT_VALUE ||
			gWaterSensor.sensorVoltage > MAX_VOLT_VALUE)
	{
	    return WATER_SENSOR_DEFECT;
	}

	int32_t deltaVoltage = gWaterSensor.sensorVoltage - VOLT_OFFSET;

	*waterlevel = MIN_SENSOR_VALUE + (deltaVoltage / gWaterSensor.conversionFactor);

	if(*waterlevel < MIN_SENSOR_VALUE || *waterlevel > MAX_SENSOR_VALUE)
	{
	    return WATER_SENSOR_INVALID_VALUE;
	}

	//gWaterSensor.newValueAvailable = false;
	return WATER_SENSOR_OK;
}

/**
 * @brief Receives and validates UART data packets of the water sensor.
 *
 * The function polls the UART interface for available data. If bytes are
 * available, they are read one by one and appended to the internal packet
 * buffer. As soon as a full packet has been received, the following checks
 * are performed:
 * - CRC validation
 * - packet counter increment by one
 * - 8-bit packet counter roll-over handling
 *
 * If the packet is valid, the contained sensor voltage is stored in the
 * internal water sensor instance.
 *
 * This function is intended to be called cyclically from the 10 ms task.
 *
 * @return `WATER_SENSOR_OK` if no error occurred.
 * @return `WATER_SENSOR_DEFECT` if the CRC is invalid or the packet counter
 *         is not incremented correctly.
 * @return `WATER_SENSOR_ERR` if the module is not initialized or UART
 *         reception failed.
 */
int32_t waterSensorReceiveUartData(void)
{
    if(gWaterSensor.initialized == false)
    {
        return WATER_SENSOR_ERR;
    }

    bool hasData = false;
    uint8_t rxByte = 0U;

    while(1)
    {
        if(uartHasData(&hasData) != UART_ERR_OK)
        {
            return WATER_SENSOR_ERR;
        }

        if(hasData == false)
        {
            break;
        }

        if(uartReceiveData(&rxByte, 1, 0U) != UART_ERR_OK)
        {
            return WATER_SENSOR_ERR;
        }

        gWaterSensor.rxBuffer[gWaterSensor.rxIndex] = rxByte;
        gWaterSensor.rxIndex++;

        if(gWaterSensor.rxIndex >= WATER_SENSOR_PACKET_SIZE)
        {
            gWaterSensor.rxIndex = 0U;

            if(waterSensorValidatePacket(gWaterSensor.rxBuffer) != WATER_SENSOR_OK)
            {
                return WATER_SENSOR_DEFECT;
            }

            waterSensorStorePacket(gWaterSensor.rxBuffer);
        }
    }

    return WATER_SENSOR_OK;
}

/**
 * @brief Supervises the UART timeout of the water sensor.
 *
 * The function checks whether a valid packet has been received within the
 * configured timeout interval.
 *
 * If no valid packet has been received for more than 1500 ms, the sensor
 * is considered defective.
 *
 * This function is intended to be called cyclically from the 10 ms task.
 *
 * @return `WATER_SENSOR_OK` if the timeout condition is not violated.
 * @return `WATER_SENSOR_DEFECT` if no valid packet has been received
 *         for more than 1500 ms.
 * @return `WATER_SENSOR_ERR` if the module is not initialized.
 */
int32_t waterSensorCheckTimeout(void)
{
    if(gWaterSensor.initialized == false)
    {
        return WATER_SENSOR_ERR;
    }

    if((HAL_GetTick() - gWaterSensor.lastValidPacketTick) > WATER_SENSOR_TIMEOUT_MS)
    {
        return WATER_SENSOR_DEFECT;
    }

    return WATER_SENSOR_OK;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Calculates the CRC of a water sensor packet.
 *
 * The CRC is calculated over all packet bytes except the CRC byte itself.
 *
 * This implementation uses a simple XOR checksum. If the UART protocol
 * specifies another CRC algorithm, this function must be adapted accordingly.
 *
 * @param[in] data Pointer to the packet bytes used for CRC calculation.
 * @param[in] length Number of bytes included in the CRC calculation.
 *
 * @return Calculated CRC value.
 */
static uint8_t waterSensorCalculateCrc(const uint8_t *data, uint8_t length)
{
    uint8_t crc = 0U;

    if(data == NULL)
    {
        return 0U;
    }

    for(uint8_t i = 0U; i < length; i++)
    {
        crc ^= data[i];
    }

    return crc;
}

/**
 * @brief Validates a complete received UART packet.
 *
 * The function checks:
 * - whether the CRC matches
 * - whether the packet counter is incremented by one compared to the
 *   previous valid packet
 * - whether the 8-bit packet counter roll-over is handled correctly
 *
 * The very first valid packet is accepted without counter comparison.
 *
 * @param[in] packet Pointer to the received packet buffer.
 *
 * @return `WATER_SENSOR_OK` if the packet is valid.
 * @return `WATER_SENSOR_DEFECT` if the CRC is invalid or the packet counter
 *         does not match the expected value.
 * @return `WATER_SENSOR_INVALID_PTR` if `packet` is `NULL`.
 */
static int32_t waterSensorValidatePacket(const uint8_t *packet)
{
    if(packet == NULL)
    {
        return WATER_SENSOR_INVALID_PTR;
    }

    uint8_t calculatedCrc = waterSensorCalculateCrc(packet, WATER_SENSOR_CRC_DATA_LENGTH);
    uint8_t receivedCrc = packet[WATER_SENSOR_CRC_INDEX];

    if(calculatedCrc != receivedCrc)
    {
        return WATER_SENSOR_DEFECT;
    }

    if(gWaterSensor.hasPreviousPacket == true)
    {
        uint8_t expectedCounter = (uint8_t)(gWaterSensor.lastPacketCounter + 1U);

        if(packet[WATER_SENSOR_COUNTER_INDEX] != expectedCounter)
        {
            return WATER_SENSOR_DEFECT;
        }
    }

    return WATER_SENSOR_OK;
}

/**
 * @brief Stores the contents of a validated UART packet.
 *
 * The function extracts the packet counter and raw voltage value from the
 * packet buffer and updates the internal module state.
 *
 * The received voltage is additionally written into the existing water
 * sensor instance so that the already available conversion functions can
 * continue to be used unchanged.
 *
 * @param[in] packet Pointer to the validated packet buffer.
 */
static void waterSensorStorePacket(const uint8_t *packet)
{
    uint32_t sensorVolt = 0U;

    sensorVolt  = ((uint32_t)packet[WATER_SENSOR_VOLTAGE_HIGH_INDEX] << 8U);
    sensorVolt |= ((uint32_t)packet[WATER_SENSOR_VOLTAGE_LOW_INDEX]);

    gWaterSensor.lastPacketCounter = packet[WATER_SENSOR_COUNTER_INDEX];
    gWaterSensor.hasPreviousPacket = true;
    gWaterSensor.lastValidPacketTick = HAL_GetTick();
    gWaterSensor.newValueAvailable = true;

    gWaterSensor.sensorVoltage = sensorVolt;
}

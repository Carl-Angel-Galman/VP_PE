/**
 * @file    WaterSensor.h
 * @author  Carl Angel Galman
 * @author  Liza Henriette Thöne
 * @date    03.03.2026
 *
 * @brief   Interface for handling a water level sensor.
 *
 * This module provides function declarations to:
 * - initialize the water sensor instance
 * - update the measured sensor voltage
 * - convert the measured voltage into a water level value
 * - receive and validate UART packets
 * - supervise packet timeout
 *
 * The sensor voltage is expected in microvolts and is converted into
 * a water level in centimeters using a fixed conversion factor.
 */

#ifndef SRC_SERVICE_WATERSENSOR_H_
#define SRC_SERVICE_WATERSENSOR_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Exported Defines
 ******************************************************************************/

/**
 * @brief Operation completed successfully.
 */
#define WATER_SENSOR_OK 0

/**
 * @brief Invalid pointer passed to a function.
 */
#define WATER_SENSOR_INVALID_PTR -1

/**
 * @brief Calculated water level is outside the valid range.
 */
#define WATER_SENSOR_INVALID_VALUE -3

/**
 * @brief Sensor voltage is outside the valid operating range.
 */
#define WATER_SENSOR_DEFECT -5

/**
 * @brief No valid sensor data available.
 */
#define NO_DATA_GET -7

/**
 * @brief General module error.
 */
#define WATER_SENSOR_ERR -9

/**
 * @brief No complete UART packet available yet.
 */
#define WATER_SENSOR_NO_NEW_DATA 1

/**
 * @brief Size of one UART packet in bytes.
 *
 * Packet format:
 * - Byte 0: packet counter
 * - Byte 1: sensor voltage high byte
 * - Byte 2: sensor voltage low byte
 * - Byte 3: CRC
 */
#define WATER_SENSOR_PACKET_SIZE 4U

/*******************************************************************************
 * Exported Types and Typedefs
 ******************************************************************************/

/**
 * @brief Structure representing a water sensor instance.
 */
typedef struct _WaterSensor
{
	/**
	 * @brief Stored sensor voltage in microvolts.
	 */
	uint32_t sensorVoltage;

	/**
	 * @brief Conversion factor used to convert voltage to water level.
	 *
	 * Unit: microvolts per centimeter.
	 */
	uint32_t conversionFactor;

    uint8_t rxBuffer[WATER_SENSOR_PACKET_SIZE];
    uint8_t rxIndex;

    uint8_t lastPacketCounter;
    bool hasPreviousPacket;

    uint32_t lastValidPacketTick;
    bool initialized;
    bool newValueAvailable;

} WaterSensor;

/*******************************************************************************
 * Exported Functions
 ******************************************************************************/

/**
 * @brief Initializes the water sensor instance.
 *
 * The function resets the stored sensor voltage and assigns the
 * predefined conversion factor used for voltage-to-level conversion.
 *
 * @return `WATER_SENSOR_OK` if initialization was successful.
 */
int32_t waterSensorInitalize(void);

/**
 * @brief Updates the stored sensor voltage.
 *
 * The function reads the current sensor voltage, validates it, and stores
 * it in the sensor instance.
 *
 * @return `WATER_SENSOR_OK` if the voltage was updated successfully.
 * @return `WATER_SENSOR_DEFECT` if the measured voltage lies outside the
 *         valid operating range.
 */
int32_t waterSensorSetSensorVoltage(void);

/**
 * @brief Returns the calculated water level.
 *
 * The function converts the stored sensor voltage into a water level
 * in centimeters.
 *
 * @param[out] waterlevel Pointer to the variable that receives the
 *                        calculated water level.
 *
 * @return `WATER_SENSOR_OK` if the water level was calculated successfully.
 * @return `WATER_SENSOR_INVALID_PTR` if `waterlevel` is `NULL`.
 * @return `WATER_SENSOR_INVALID_VALUE` if the calculated water level lies
 *         outside the valid range.
 * @return `WATER_SENSOR_DEFECT` if the stored voltage is invalid.
 */
int32_t waterSensorGetSensorValue(int32_t* waterlevel);

/**
 * @brief Receives and validates UART data of the water sensor.
 *
 * The function polls the UART interface for new bytes and assembles them
 * into complete sensor packets.
 *
 * For every complete packet, the following checks are performed:
 * - CRC validation
 * - packet counter increment by one
 * - correct 8-bit roll-over behavior of the packet counter
 *
 * If a packet is valid, the contained voltage value is stored internally.
 *
 * This function is intended to be called cyclically from the 10 ms task
 * while the application is in Operational state.
 *
 * @return `WATER_SENSOR_OK` if no error occurred.
 * @return `WATER_SENSOR_DEFECT` if the CRC is invalid or the packet counter
 *         is not incremented correctly.
 * @return `WATER_SENSOR_ERR` if UART reception failed or the module is not
 *         initialized.
 */
int32_t waterSensorReceiveUartData(void);

/**
 * @brief Checks whether the water sensor UART reception timed out.
 *
 * The function verifies whether a valid UART packet has been received
 * within the configured timeout interval.
 *
 * If no valid packet has been received for more than 1500 ms, the sensor
 * is considered defective.
 *
 * This function is intended to be called cyclically from the 10 ms task
 * while the application is in Operational state.
 *
 * @return `WATER_SENSOR_OK` if the timeout condition is not violated.
 * @return `WATER_SENSOR_DEFECT` if no valid packet has been received
 *         within the allowed timeout interval.
 * @return `WATER_SENSOR_ERR` if the module is not initialized.
 */
int32_t waterSensorCheckTimeout(void);



#endif /* SRC_SERVICE_WATERSENSOR_H_ */

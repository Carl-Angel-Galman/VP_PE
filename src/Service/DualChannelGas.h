/*
 * DualChannelGas.h
 *
 *  Created on: Feb 24, 2026
 *      Author: kali
 */

#ifndef SRC_SERVICE_DUALCHANNELGAS_H_
#define SRC_SERVICE_DUALCHANNELGAS_H_

#include <stdint.h>
#include "GasSensor.h"
#include "stm32g4xx_hal.h"

#define DUALSENSORS_OK 0
#define DUALSENSORS_NOT_OK -10
#define DUALSENSORS_DEFECT -11
#define DUALSENSORS_INVALID_PTR -12

//Init the two gasSensors and the ADCperipherie of the module, return SENSORS_OK, if no error occured
int32_t dualGasInit(void);

//Reads the ADC Input and set the Voltage, return SENSORS_OK, if no error occured
int32_t dualGasSetVoltages(void);

int32_t dualGasCheckInconsistency(void);

int32_t dualGasGetAverage(int32_t * average);



#endif /* SRC_SERVICE_DUALCHANNELGAS_H_ */

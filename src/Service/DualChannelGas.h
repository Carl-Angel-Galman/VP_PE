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

#define SENSORS_OK 0
#define SENSORS_NOT_OK -10
#define SENSORS_DEFECT -11


//Init the two gasSensors and the ADCperipherie of the module, return SENSORS_OK, if no error occured
int32_t dualGasInit();

//Reads the ADC Input and set the Voltage, return SENSORS_OK, if no error occured
int32_t dualGasSetVoltages();

int32_t dualGasCheckInconsistency();
int32_t dualGasGetAverage();



#endif /* SRC_SERVICE_DUALCHANNELGAS_H_ */

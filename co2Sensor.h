
#include "global_declarations.h"
#include <ATMEGA_FreeRTOS.h>
#include <mh_z19.h>
#include <time.h>

#ifndef CO2SENSOR_H_
#define CO2SENSOR_H_

// Fields
uint16_t lastCO2ppm;
mh_z19_returnCode_t rc;



// Functions
void CO2_handler_create();
void co2sensorTask(void* pvParameters);
uint16_t getCO2();
void mh_z19_callBack(uint16_t ppm);

#endif /* CO2SENSOR_H_ */
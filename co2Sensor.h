#pragma  once
#ifndef CO2SENSOR_H_
#define CO2SENSOR_H_
//----------------------------INCLUDES----------------------------

#include "global.h"
#include <mh_z19.h>
#include <time.h>
//-----------------------------------------------------------------


//----------------------------PARAMETERS----------------------------
uint16_t lastCO2ppm;
mh_z19_returnCode_t rc;
//-----------------------------------------------------------------


//--------------------------FUNCTIONS------------------------------
void CO2_handler_create();
void co2sensorTask(void* pvParameters);
void mh_z19_callBack(uint16_t ppm);
uint16_t getCO2();
void waitFor (unsigned int secs);
//-----------------------------------------------------------------


#endif
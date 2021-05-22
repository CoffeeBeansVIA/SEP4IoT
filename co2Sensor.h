#pragma  once
#ifndef CO2SENSOR_H_
#define CO2SENSOR_H_
//----------------------------INCLUDES----------------------------
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <mh_z19.h>
#include <time.h>
#include <event_groups.h>
//----------------------------------------------------------------- 


//----------------------------EVENT GROUPS-------------------------
EventGroupHandle_t measureEventGroup;
EventGroupHandle_t readyEventGroup;
//-----------------------------------------------------------------


//----------------------------MUTEXES------------------------------
void mutexPuts(char* str);
//-----------------------------------------------------------------


//----------------------------DEFINES------------------------------
#define BIT_TASK_CO2_MEASURE (1 << 0)
#define BIT_TASK_CO2_READY (1 << 1)
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

#include <ATMEGA_FreeRTOS.h>
#include <mh_z19.h>
#include <time.h>
#include <event_groups.h>

#ifndef CO2SENSOR_H_
#define CO2SENSOR_H_

EventGroupHandle_t measureEventGroup;
#define BIT_TASK_CO2_MEASURE (1<<0)
EventGroupHandle_t readyEventGroup;
#define BIT_TASK_CO2_READY (1<<1)

// Fields
uint16_t lastCO2ppm;
mh_z19_returnCode_t rc;



// Functions
void CO2_handler_create();
void co2sensorTask(void* pvParameters);
uint16_t getCO2();
void mh_z19_callBack(uint16_t ppm);

#endif /* CO2SENSOR_H_ */
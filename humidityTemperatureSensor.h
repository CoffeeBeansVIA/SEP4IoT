#pragma once
#ifndef HUMIDITYTEMPERATURESENSOR_H_
#define HUMIDITYTEMPERATURESENSOR_H_
//----------------------------INCLUDES----------------------------
#include "global.h"
#include <hih8120.h>

//----------------------------TASK---------------------------------
TaskHandle_t _temperatureHumiditySensorTask; //i deleted static
//-----------------------------------------------------------------


//------------------------PRIVATE FIELDS---------------------------- i deleted static
float _lastTemperature;
float _lastHumidity;
//-----------------------------------------------------------------


//----------------------------FUNCTIONS----------------------------
void temperatureHumiditySensor_create();
void HumidityTemperatureSensorTask(void *pvParameters);
float humidityTemperatureSensor_getHumidity();
float humidityTemperatureSensor_getTemperature();
void _setup_temperature_humidity_driver();
//-----------------------------------------------------------------

#endif
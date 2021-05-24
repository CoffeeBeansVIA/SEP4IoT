#pragma once
#ifndef HUMIDITYTEMPERATURESENSOR_H_
#define HUMIDITYTEMPERATURESENSOR_H_
//----------------------------INCLUDES----------------------------
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <event_groups.h>
#include <task.h>
#include <hih8120.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio_driver.h>
#include <avr/io.h>
//-----------------------------------------------------------------


//----------------------------DEFINES---------------------------------
#define HUMIDITY_TEMPERATURE_MEASURE_BIT (1 << 0)
#define HUMIDITY_TEMPERATURE_READY_BIT (1 << 1)
//-----------------------------------------------------------------


//----------------------------TASK---------------------------------
static TaskHandle_t _temperatureHumiditySensorTaskHandle;
//-----------------------------------------------------------------


//------------------------PRIVATE FIELDS----------------------------
static SemaphoreHandle_t _xPrintfSemaphore;
static EventGroupHandle_t _eventGroupHandleMeasure;
static EventGroupHandle_t _eventGroupHandleNewData;
static float _lastTemperature;
static float _lastHumidity;
//-----------------------------------------------------------------

 
//----------------------------FUNCTIONS----------------------------
void temperatureHumiditySensor_create(EventGroupHandle_t pvEventHandleMeasure,
EventGroupHandle_t pvEventHandleNewData, SemaphoreHandle_t pPrintfSemaphore);
float humidityTemperatureSensor_getHumidity();
float humidityTemperatureSensor_getTemperature();
//-----------------------------------------------------------------

#endif
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

/*
 * co2Sensor.c
 *
 * Created: 23/04/2021 14.41.21
 *  Author: tanki
 */ 


uint16_t lastCO2ppm;

void create(void* pvParameters){
		(void)pvParameters;
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
		
	for (;;) {
		
		
	}
} 

void mh_z19_callBack(uint16_t ppm){
	
}
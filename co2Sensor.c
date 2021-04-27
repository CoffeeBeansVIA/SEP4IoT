#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <mh_z19.h>
#include <time.h>
#include <event_groups.h>

/*
 * co2Sensor.c
 *
 * Created: 23/04/2021 14.41.21
 *  Author: tanki
 */ 

void mh_z19_callBack(uint16_t ppm);
uint16_t lastCO2ppm;
mh_z19_returnCode_t rc;

EventGroupHandle_t measureEventGroup;
EventGroupHandle_t readyEventGroup;

#define BIT_TASK_CO2_MEASURE (1 << 0)
#define BIT_TASK_CO2_READY (1 << 1)
 

void create(void* pvParameters){
	(void)pvParameters;
		// The parameter is the USART port the MH-Z19 sensor is connected to - in this case USART3
		mh_z19_initialise(ser_USART3);
		mh_z19_injectCallBack(mh_z19_callBack);
	
} 

void co2sensorTask(void* pvParameters)
{
	(void)pvParameters;
	while(1){
		xEventGroupWaitBits(
		measureEventGroup,
		BIT_TASK_CO2_MEASURE,
		pdTRUE,
		pdTRUE,
		portMAX_DELAY);
		
		rc = mh_z19_takeMeassuring();
		if (rc != MHZ19_OK)
		{
			puts("something went wrong in co2Sensor");
			// Something went wrong
		}
	}
}

void mh_z19_callBack(uint16_t ppm){
	lastCO2ppm = ppm;
	xEventGroupSetBits(readyEventGroup,BIT_TASK_CO2_READY);
}

uint16_t getCO2(){
	
	return lastCO2ppm;
}

void waitFor (unsigned int secs) {
	unsigned int retTime = time(0) + secs;   // Get finishing time.
	while (time(0) < retTime);               // Loop until it arrives.
}
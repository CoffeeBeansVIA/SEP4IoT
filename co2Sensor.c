#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include <mh_z19.h>
#include <time.h>
#include "event_groups.h"

/*
 * co2Sensor.c
 *
 * Created: 23/04/2021 14.41.21
 *  Author: tanki
 */ 


uint16_t lastCO2ppm;
mh_z19_returnCode_t rc;

EventGroupHandle_t xMeasureEventGroup;
EventGroupHandle_t xDataReadyEventGroup;

xMeasureEventGroup = xEventGroupCreate();
xDataReadyEventGroup = xEventGroupCreate();


xEventGroupWaitBits();
 

void create(void* pvParameters){
	(void)pvParameters;
		// The parameter is the USART port the MH-Z19 sensor is connected to - in this case USART3
		mh_z19_initialise(ser_USART3);
		mh_z19_injectCallBack(mh_z19_callBack);
	
} 

void mh_z19_callBack(uint16_t ppm){
	
	waitFor(1);
	rc = mh_z19_takeMeassuring();
	if (rc != mh_z19_returnCode_t.MHZ19_OK)
	{
		puts("something went wrong in co2Sensor")
		// Something went wrong
	}
	lastCO2ppm = ppm;
}

uint16_t getCO2(){
	
	return lastCO2ppm;
}

void waitFor (unsigned int secs) {
	unsigned int retTime = time(0) + secs;   // Get finishing time.
	while (time(0) < retTime);               // Loop until it arrives.
}
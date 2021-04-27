#include "SensorDataPackage.h"
#include <ATMEGA_FreeRTOS.h>
#include <stdint.h>

typedef struct SensorDataPackage {
	uint16_t co2Data;
} SensorDataPackage_st;

SensorDataPackage_t SensorDataPackage_create(){
	SensorDataPackage_t packet = pvPortMalloc(sizeof(SensorDataPackage_st));
	
	if (NULL == packet){
		return NULL;
	}
	
	return packet;
}

void SensorDataPackage_free(SensorDataPackage_t packet){
	vPortFree( (void *) packet );
	packet = NULL;
}

void SensorDataPackage_setCO2(SensorDataPackage_t packet, uint16_t _co2Data){
	packet->co2Data = _co2Data;
}

uint16_t SensorDataPackage_getCO2(SensorDataPackage_t packet){
	return packet->co2Data;
}
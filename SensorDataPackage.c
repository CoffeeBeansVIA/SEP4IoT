#include "SensorDataPackage.h"

typedef struct {
	uint16_t co2Data;
} SensorDataPackage;

SensorDataPackage_t SensorDataPackage_create(uint16_t _co2Data){
	SensorDataPackage_t packet = calloc(sizeof(SensorDataPackage_t), 1);
	
	if (NULL == packet){
		return NULL;
	}
	
	packet->co2Data = _co2Data;
	
	return packet;
}


uint16_t SensorDataPackage_getCO2(SensorDataPackage packet){
	return packet->co2Data;
}
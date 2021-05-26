//----------------------------INCLUDES----------------------------
#include "SensorDataPackage.h"
//-----------------------------------------------------------------


//----------------------------STRUCT------------------------------
typedef struct SensorDataPackage {
	uint16_t co2Data;
	float tempData;
	float humidityData;
} SensorDataPackage_st;
//-----------------------------------------------------------------


//----------------------------FUNCTIONS----------------------------
SensorDataPackage_t SensorDataPackage_create(){
	SensorDataPackage_t packet = pvPortMalloc(sizeof(SensorDataPackage_st));
	packet->co2Data = -1;
	packet->tempData = -1.0;
	packet->humidityData = -1.0;
	
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

//new sensors
float SensorDataPackage_getTemperature(SensorDataPackage_t packet){
	return packet->tempData;
}
float SensorDataPackage_getHumidity(SensorDataPackage_t packet){
	return packet->humidityData;
}
void SensorDataPackage_setTemperature(SensorDataPackage_t packet, float _tempData){
	packet->tempData = _tempData;
}
void SensorDataPackage_setHumidity(SensorDataPackage_t packet, float _humidityData){
	packet->humidityData = _humidityData;
}
//-----------------------------------------------------------------

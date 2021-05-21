#include "ATMEGA_FreeRTOS.h"
#include "semphr.h"
#include "Configuration.h"
#include <stdint.h>


typedef struct Configuration {
	uint16_t co2Data;
	uint16_t co2MaxFluct;
	uint16_t temperatureData;
	uint16_t temperatureMaxFluct;
	uint16_t humidityData;
	uint16_t humidityMaxFluct;
} Configuration_st;

Configuration_t Configuration_create(){
	Configuration_t config = pvPortMalloc(sizeof(Configuration_t));
	config->co2Data = 700;
	if (NULL == config){
		return NULL;
	}
	return config;
}


void Configuration_free(Configuration_t config){
	vPortFree( (void *) config );
	config = NULL;
}

void Configuration_setCO2(Configuration_t config, uint16_t _co2Data){
	
	config->co2Data = _co2Data;
	puts("Co2 set to %d \n",_co2Data);
}

void Configuration_setCO2MaxFluct(Configuration_t config, uint16_t _co2MaxFluct){
	config->co2MaxFluct = _co2MaxFluct;
	puts("Co2 Maximum Fluctuation set to %d \n",_co2MaxFluct);
}

void Configuration_setTemperature(Configuration_t config, uint16_t _temperatureData){
	config->temperatureData = _temperatureData;
}
void Configuration_setTemperatureMaxFluct(Configuration_t config, uint16_t _temperatureMaxFluct){
	config->temperatureMaxFluct = _temperatureMaxFluct;
}
void Configuration_setHumidity(Configuration_t config, uint16_t _humidityData){
	config->humidityData = _humidityData;
}
void Configuration_setHumidityMaxFluct(Configuration_t config, uint16_t _humidityMaxFluct){
	config->humidityData = _humidityMaxFluct;
}

uint16_t Configuration_getCO2(Configuration_t config){
	return config->co2Data;
}
uint16_t Configuration_getTemperature(Configuration_t config){
	return config->temperatureData;
}
uint16_t Configuration_getHumidity(Configuration_t config){
	return config->humidityData;
}

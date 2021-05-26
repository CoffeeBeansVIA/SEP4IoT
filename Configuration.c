//----------------------------INCLUDES----------------------------
#include "Configuration.h"
//-----------------------------------------------------------------


//----------------------------STRUCT------------------------------
typedef struct Configuration {
	uint16_t co2Data;
	uint16_t co2MaxFluct;
	uint16_t temperatureData;
	uint16_t temperatureMaxFluct;
	uint16_t humidityData;
	uint16_t humidityMaxFluct;
} Configuration_st;
//-----------------------------------------------------------------


//----------------------------FUNCTIONS------------------------------
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
	char buff[63];
	sprintf(buff, "Co2 set to %d \n",_co2Data);
	mutexPuts(buff);
}

void Configuration_setCO2MaxFluct(Configuration_t config, uint16_t _co2MaxFluct){
	config->co2MaxFluct = _co2MaxFluct;
	char buff[63];	
	sprintf(buff, "Co2 Maximum Fluctuation set to %d \n",_co2MaxFluct);
	mutexPuts(buff);
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
//new sensors
void Configuration_setTemperature(Configuration_t config, uint16_t _temperatureData){
	config->temperatureData = _temperatureData;
	char buff[63];
	sprintf(buff, "Temperature set to %d \n",_temperatureData);
	mutexPuts(buff);
}
void Configuration_setTemperatureMaxFluct(Configuration_t config, uint16_t _temperatureMaxFluct){
	config->temperatureMaxFluct = _temperatureMaxFluct;
	char buff[63];
	sprintf(buff, "Temperature Maximum Fluctuation set to %d \n",_temperatureMaxFluct);
	mutexPuts(buff);
}
void Configuration_setHumidity(Configuration_t config, uint16_t _humidityData){
	config->humidityData = _humidityData;
	char buff[63];
	sprintf(buff, "Humidity set to %d \n",_humidityData);
	mutexPuts(buff);
}
void Configuration_setHumidityMaxFluct(Configuration_t config, uint16_t _humidityMaxFluct){
	config->humidityData = _humidityMaxFluct;
	char buff[63];
	sprintf(buff, "Humidity Maximum Fluctuation set to %d \n",_humidityMaxFluct);
	mutexPuts(buff);
}


//-----------------------------------------------------------------

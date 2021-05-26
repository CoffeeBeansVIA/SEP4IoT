#pragma  once
#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
//----------------------------INCLUDES----------------------------
#include "global.h"
//-----------------------------------------------------------------


//-------------------DEFINE TYPE CONFIGURATION---------------------
typedef struct Configuration* Configuration_t;
//-----------------------------------------------------------------
Configuration_t configuration;

//----------------------------FUNCTIONS----------------------------
Configuration_t Configuration_create();
void Configuration_destroy(Configuration_t config);
void Configuration_setCO2(Configuration_t config, uint16_t _co2Data);
void Configuration_setCO2MaxFluct(Configuration_t config, uint16_t _co2Data);
void Configuration_setTemperature(Configuration_t config, uint16_t _temperatureData);
void Configuration_setTemperatureMaxFluct(Configuration_t config, uint16_t _tempData);
void Configuration_setHumidityMaxFluct(Configuration_t config, uint16_t _humidityMaxFluct);
void Configuration_setHumidity(Configuration_t config, uint16_t _humidityData);
uint16_t Configuration_getCO2(Configuration_t config);
void Configuration_free(Configuration_t config); 
//-----------------------------------------------------------------


#endif
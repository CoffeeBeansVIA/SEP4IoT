#pragma once
#include <stdint.h>


//define type 'Configuration'
typedef struct Configuration* Configuration_t;

Configuration_t Configuration_create();
void Configuration_destroy(Configuration_t config);

void Configuration_setCO2(Configuration_t config, uint16_t _co2Data);
void Configuration_setTemperature(Configuration_t config, uint16_t _temperatureData);
void Configuration_setHumidity(Configuration_t config, uint16_t _humidityData);
uint16_t Configuration_getCO2(Configuration_t config);
void Configuration_free(Configuration_t config); 

#pragma once
#include <stdint.h>

typedef struct SensorDataPackage* SensorDataPackage_t;

SensorDataPackage_t SensorDataPackage_create();
void SensorDataPackage_destroy(SensorDataPackage_t packet);
void SensorDataPackage_setCO2(SensorDataPackage_t packet, uint16_t _co2Data);
void SensorDataPackage_setTemperature(SensorDataPackage_t packet, uint16_t _temperatureData);
void SensorDataPackage_setHumidity(SensorDataPackage_t packet, uint16_t _humidityData);
uint16_t SensorDataPackage_getCO2(SensorDataPackage_t packet);
void SensorDataPackage_free(SensorDataPackage_t packet);
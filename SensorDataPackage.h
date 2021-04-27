#pragma once
#include <stdint.h>

typedef struct SensorDataPackage* SensorDataPackage_t;

SensorDataPackage_t SensorDataPackage_create();
void SensorDataPackage_destroy(SensorDataPackage_t packet);
void SensorDataPackage_setCO2(SensorDataPackage_t packet, uint16_t _co2Data);
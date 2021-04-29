#include "ATMEGA_FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t temperatureMutex;
SemaphoreHandle_t co2Mutex;
SemaphoreHandle_t humidityMutex;
void create_configuration();
SemaphoreHandle_t setTemperature(SemaphoreHandle_t source);
SemaphoreHandle_t getTemperature();
SemaphoreHandle_t setCo2(SemaphoreHandle_t source);
SemaphoreHandle_t getCo2();
SemaphoreHandle_t setHumidity(SemaphoreHandle_t source);
SemaphoreHandle_t getHumidity();
#include "ATMEGA_FreeRTOS.h"
#include "semphr.h"
#include "Configuration.h"


void create_configuration(){
	temperatureMutex = xSemaphoreCreateMutex();
	co2Mutex = xSemaphoreCreateMutex();
	humidityMutex = xSemaphoreCreateMutex();
}

SemaphoreHandle_t setTemperature(SemaphoreHandle_t source)
{
	temperatureMutex = source;
}
SemaphoreHandle_t getTemperature(){
	return temperatureMutex;
}
SemaphoreHandle_t setCo2(SemaphoreHandle_t source)
{
	co2Mutex = source;
}
SemaphoreHandle_t getCo2(){
	return co2Mutex;
}
SemaphoreHandle_t setHumidity(SemaphoreHandle_t source)
{
	humidityMutex = source;
}
SemaphoreHandle_t getHumidity(){
	return humidityMutex;
}
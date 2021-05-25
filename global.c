#include "global.h"

int UpLinkSize = sizeof(SensorDataPackage_t)*2;
int DownLinkSize  = sizeof(lora_driver_payload_t)*2;

void mutexPuts(char* str){
	if(xSemaphoreTake(putsMutex, portMAX_DELAY) == pdTRUE){
		puts(str);
		xSemaphoreGive(putsMutex);
	}
}

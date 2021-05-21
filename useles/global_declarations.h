
#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdio.h>;
#include <avr/io.h>;
#include <event_groups.h>;
#include "semphr.h";
#include <task.h>;


// Mutexes
SemaphoreHandle_t measureCo2Mutex;
SemaphoreHandle_t UpLinkSendMutex;
SemaphoreHandle_t UpLinkReceiveMutex;
SemaphoreHandle_t putsMutex;
SemaphoreHandle_t sysInitMutex;

// Event groups
EventGroupHandle_t measureEventGroup = NULL;
#define BIT_TASK_CO2_MEASURE (1<<0);
EventGroupHandle_t readyEventGroup = NULL;
#define BIT_TASK_CO2_READY (1<<1);

void mutexPuts(char* str){
	if(xSemaphoreTake(putsMutex, portMAX_DELAY) == pdTRUE){
		printf(str);
		xSemaphoreGive(putsMutex);
	}
}

#endif /* GLOBALS_H_ */
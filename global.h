#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio_driver.h>
#include <avr/io.h>
#include <serial.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <message_buffer.h>
#include <event_groups.h>
#include <lora_driver.h>
#include <status_leds.h>

#include "SensorDataPackage.h"
#include "Configuration.h"

// Mutexes
SemaphoreHandle_t sysInitMutex;
SemaphoreHandle_t measureCo2Mutex;
SemaphoreHandle_t measureTempMutex;
SemaphoreHandle_t UpLinkSendMutex;
SemaphoreHandle_t UpLinkReceiveMutex;
SemaphoreHandle_t putsMutex;
SemaphoreHandle_t windowMutex;
//SemaphoreHandle_t DownLinkReceiveUpdateMutex;

void mutexPuts(char* str);

// Event groups
EventGroupHandle_t measureEventGroup;
EventGroupHandle_t readyEventGroup;

#define BIT_TASK_CO2_MEASURE (1<<0)
#define BIT_TASK_CO2_READY (1<<1)

#define HUMIDITY_TEMPERATURE_MEASURE_BIT (1 << 0)
#define HUMIDITY_TEMPERATURE_READY_BIT (1 << 1)

// MessageBuffers
int UpLinkSize;
int DownLinkSize;
MessageBufferHandle_t UpLinkMessageBuffer;
MessageBufferHandle_t DownLinkMessageBuffer;
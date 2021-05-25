#pragma once

#include <stdint.h>
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
SemaphoreHandle_t UpLinkSendMutex;
SemaphoreHandle_t UpLinkReceiveMutex;
SemaphoreHandle_t putsMutex;
SemaphoreHandle_t windowMutex;
//SemaphoreHandle_t DownLinkReceiveUpdateMutex;

void mutexPuts(char* str);

// Event groups
EventGroupHandle_t measureEventGroup;
#define BIT_TASK_CO2_MEASURE (1<<0)
EventGroupHandle_t readyEventGroup;
#define BIT_TASK_CO2_READY (1<<1)

// MessageBuffers
int UpLinkSize;
int DownLinkSize;
static MessageBufferHandle_t UpLinkMessageBuffer;
static MessageBufferHandle_t DownLinkMessageBuffer;
#pragma  once
#ifndef UPLINKHANDLER_H_
#define UPLINKHANDLER_H_
//----------------------------INCLUDES----------------------------
#include "upLinkHandler.h"
#include "ATMEGA_FreeRTOS.h"
#include <stdio.h>
#include <lora_driver.h>
#include <message_buffer.h>
#include <status_leds.h>
#include <semphr.h>
#include "SensorDataPackage.h"
//-----------------------------------------------------------------


//----------------------------MUTEXES----------------------------
void mutexPuts(char* str);
SemaphoreHandle_t UpLinkReceiveMutex;
SemaphoreHandle_t UpLinkSendMutex;
//-----------------------------------------------------------------


//-----------------------------------------------------------------
const int UpLinkSize;
static char _out_buf[100];
//static lora_driver_payload_t uplink_payload;
MessageBufferHandle_t UpLinkMessageBuffer;
//-----------------------------------------------------------------


//-----------PARAMETERS FOR OTAA JOIN (FROM IHA)-------------------
#define LORA_appEUI "926F9B5931FCA94C"
#define LORA_appKEY "1D2EB57B831FBDEF807978AE930786E4"
//-----------------------------------------------------------------


//----------------------------FUNCTIONS----------------------------
void UL_handler_create();
void UL_receive_task( void *pvParameters );
static void _lora_setup();
//-----------------------------------------------------------------


#endif

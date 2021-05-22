#pragma  once
#ifndef DOWNLINKHANDLER_H_
#define DOWNLINKHANDLER_H_
//----------------------------INCLUDES----------------------------
#include <lora_driver.h>
#include <status_leds.h>
#include <stdio.h>
#include <message_buffer.h>
#include "ATMEGA_FreeRTOS.h"
#include <semphr.h>
#include <stdint.h>
#include <stdint-gcc.h>
#include "Configuration.h"
#include "DownLinkHandler.h"
//-----------------------------------------------------------------


//----------------------------MUTEXES----------------------------
void mutexPuts(char* str);
SemaphoreHandle_t DownLinkReceiveUpdateMutex;
//-----------------------------------------------------------------


//-----------PARAMETERS FOR OTAA JOIN (FROM IHA)-------------------
#define LORA_appEUI "926F9B5931FCA94C"
#define LORA_appKEY "1D2EB57B831FBDEF807978AE930786E4"
//-----------------------------------------------------------------


//-----------------------------------------------------------------
Configuration_t configuration;
//static lora_driver_payload_t downlink_payload;
MessageBufferHandle_t DownLinkMessageBuffer;
const int DownLinkSize;
//-----------------------------------------------------------------


//-----VARIABLES TO TEMPORARY STORE DATA RECEIVED FROM LORAWAN-----
uint16_t maxHumSetting;
uint16_t maxTempSetting;
uint16_t maxCo2Setting;
uint16_t maxCO2Fluct;
uint16_t maxTempFluct;
uint16_t maxHumFluct;
//-----------------------------------------------------------------


//----------------------------FUNCTIONS----------------------------
void DL_handler_create();
void DL_receive_update_task(void *pvParameters);
//-----------------------------------------------------------------


#endif
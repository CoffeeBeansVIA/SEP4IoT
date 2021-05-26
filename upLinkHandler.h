#pragma  once
#ifndef UPLINKHANDLER_H_
#define UPLINKHANDLER_H_
//----------------------------INCLUDES----------------------------
#include "global.h"
#include "co2Sensor.h"

//-----------PARAMETERS FOR OTAA JOIN (FROM IHA)-------------------
#define LORA_appEUI "926F9B5931FCA94C"
#define LORA_appKEY "1D2EB57B831FBDEF807978AE930786E4"

//----------------------------FUNCTIONS----------------------------
void UL_handler_create();
void UL_receive_task( void *pvParameters );
void Welcome_task(void *pvParams);
void _lora_setup();

//-----------------------------------------------------------------


#endif

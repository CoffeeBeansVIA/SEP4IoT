#include <stdio.h>
#include <lora_driver.h>
#include <message_buffer.h>
#include "ATMEGA_FreeRTOS.h"
#include <status_leds.h>
#include <semphr.h>
#include <stdint.h>
#include "Configuration.h"
#include <stdint-gcc.h>

Configuration_t configuration;

//static lora_driver_payload_t downlink_payload;
MessageBufferHandle_t DownLinkMessageBuffer;

//variables to temporary store data received from loraWan
uint16_t maxHumSetting;
uint16_t maxTempSetting;
uint16_t maxCo2Setting;
uint16_t maxCO2Fluct;


// Mutex to print stuff in console
void mutexPuts(char* str);
SemaphoreHandle_t DownLinkReceiveUpdateMutex;

const int DownLinkSize;

// Parameters for OTAA join - You have got these in a mail
#define LORA_appEUI "926F9B5931FCA94C"
#define LORA_appKEY "1D2EB57B831FBDEF807978AE930786E4"

static char _out_buf[100];

//Functions

static void _lora_setup(void);
void DL_receive_update_task(void *pvParameters);

//create and schedule this task somewhere in main method
void DL_handler_create()
{
	xTaskCreate(
	DL_receive_update_task
	,  "DownLink Handler Receive"
	,  configMINIMAL_STACK_SIZE
	,  NULL
	,  3
	,  NULL );
	
}

void DL_receive_update_task(void *pvParameters)
{
	
	size_t xReceivedBytes;
	const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );
	int size = sizeof(configuration);
	for(;;)
	{
		//xSemaphoreTake();
		
		lora_driver_payload_t _downlink_payload;
		
		xReceivedBytes = xMessageBufferReceive( 
		DownLinkMessageBuffer,
		&_downlink_payload,
		5,
		xBlockTime
		);
		
		if(xReceivedBytes>0)
		{	
			
			printf("DOWN LINK: from port: %d with %d bytes received!", _downlink_payload.portNo, _downlink_payload.len); // Just for debug
			if (5 == _downlink_payload.len) // Check that we have got the expected 5 bytes
			{
				//decode the payload into our variables
				
				maxCo2Setting = (_downlink_payload.bytes[0] << 8) + _downlink_payload.bytes[1];
				maxCO2Fluct = (_downlink_payload.bytes[2] << 8) + _downlink_payload.bytes[3];
				//maxHumSetting = (_downlink_payload.bytes[0] << 8) + _downlink_payload.bytes[1];
				//maxTempSetting = (_downlink_payload.bytes[2] << 8) + _downlink_payload.bytes[3];
				
				//set new values
				Configuration_setCO2(configuration, maxCo2Setting);
				Configuration_setCO2MaxFluct(configuration,maxCO2Fluct);
				
				mutexPuts("new value set in configuration \n");
				char buff[63];
				sprintf(buff, "new co2 value is %d \n", Configuration_getCO2(configuration));
				mutexPuts(buff);
				
				vTaskDelay(pdMS_TO_TICKS(300000));
			
			}
			else
			{
				// Wait 2.5min to retry
				vTaskDelay(pdMS_TO_TICKS(150000));
			}
		
		}
	}
}




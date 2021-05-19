#include <stdio.h>
#include <lora_driver.h>
#include <message_buffer.h>
#include "ATMEGA_FreeRTOS.h"
#include <status_leds.h>
#include <semphr.h>
#include <stdint.h>
#include "Configuration.h"
#include <stdint-gcc.h>

//static lora_driver_payload_t downlink_payload;
MessageBufferHandle_t DownLinkMessageBuffer;

//variables to temporarly store data received from loraWan
uint16_t maxHumSetting;
uint16_t maxTempSetting;
uint16_t maxCo2Setting;


// Mutex to print stuff in console
void mutexPuts(char* str);
SemaphoreHandle_t DownLinkReceiveMutex;
SemaphoreHandle_t DownLinkUpdateMutex;

const int DownLinkSize;

// Parameters for OTAA join - You have got these in a mail
#define LORA_appEUI "926F9B5931FCA94C"
#define LORA_appKEY "1D2EB57B831FBDEF807978AE930786E4"

static char _out_buf[100];

//Functions

static void _lora_setup(void);
void DL_update_task (void *pvParameters);

//create and schedule this task somewhere in main method
void DL_handler_create()
{
	xTaskCreate(
	DL_update_task
	,  "DownLink Handler Update"
	,  configMINIMAL_STACK_SIZE
	,  NULL
	,  3
	,  NULL );
}


//This task will set config data like 'desired co2 level' in Configuration class
void DL_update_task( void *pvParameters )
{
	/*taskENTER_CRITICAL();
	// Hardware reset of LoRaWAN transceiver
	lora_driver_resetRn2483(1);
	vTaskDelay(2);
	lora_driver_resetRn2483(0);
	// Give it a chance to wakeup
	vTaskDelay(150);

	lora_driver_flushBuffers(); // get rid of first version string from module after reset!

	_lora_setup();
	taskEXIT_CRITICAL();*/

	
	for(;;)
	{
		//mutex for receiving data from loraWan
		// xSemaphoreTake( DownLinkReceiveMutex , portMAX_DELAY);
		 
		Configuration_t configuration = Configuration_create();
		 //here there will be some function receiving data from loraWan and this data shoud be saved in following variables ex: 'maxco2setting'
		
		Configuration_setCO2(configuration,maxCo2Setting);
		puts("new value set in configuration \n");
		// xSemaphoreGive(DownLinkReceiveMutex);
		 vTaskDelay(pdMS_TO_TICKS(200));
		 printf("new co2 value is %d \n", Configuration_getCO2(configuration));
		
		/* size_t xReceivedBytes;
		 const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
		 xReceivedBytes = xMessageBufferReceive( 
		 DownLinkMessageBuffer,
		 &sensorDataPackage,
		 sizeof( SensorDataPackage_t ),
		 xBlockTime
		 );*/
		 
		/*if(xReceivedBytes>0)
		if(1)
		{
			 Configuration_free(sensorDataPackage);
			 
			 lora_driver_payload_t _downlink_payload;
			
			 printf("DOWN LINK: from port: %d with %d bytes received!", _downlink_payload.portNo, _downlink_payload.len); // Just for debug
			 if (9 == _downlink_payload.len) // Check that we have got the expected 9 bytes
			 {
				 //decode the payload into our variables
				 maxCo2Setting = (_downlink_payload.bytes[4] << 8) + _downlink_payload.bytes[5];
				 maxHumSetting = (_downlink_payload.bytes[0] << 8) + _downlink_payload.bytes[1];
				 maxTempSetting = (_downlink_payload.bytes[2] << 8) + _downlink_payload.bytes[3];
				 
				 //set new values
				 SensorDataPackage_setCO2(sensorDataPackage,maxCo2Setting);
				 SensorDataPackage_setHumidity(sensorDataPackage,maxHumSetting);
				 SensorDataPackage_setTemperature(sensorDataPackage,maxTempSetting);
				
				  status_leds_shortPuls(led_ST4);  // OPTIONAL
				  printf("Updated Message >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, &_downlink_payload)));
				  vTaskDelay(300000);
				  xSemaphoreGive(DownLinkUpdateMutex);
			 }
			 else
			 {
				 // Wait 2.5min to retry
				 vTaskDelay(pdMS_TO_TICKS(150000));
			 }
			 
		}*/
	}	
	
}

 static void _lora_setup(void)
 {
	 lora_driver_returnCode_t rc;
	 status_leds_slowBlink(led_ST2); // OPTIONAL: Led the green led blink slowly while we are setting up LoRa

	 // Factory reset the transceiver
	 printf("FactoryReset >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_rn2483FactoryReset()));
	 
	 // Configure to EU868 LoRaWAN standards
	 printf("Configure to EU868 >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_configureToEu868()));

	 // Get the transceivers HW EUI
	 rc = lora_driver_getRn2483Hweui(_out_buf);
	 printf("Get HWEUI >%s<: %s\n",lora_driver_mapReturnCodeToText(rc), _out_buf);

	 // Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
	 printf("Set DevEUI: %s >%s<\n", _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setDeviceIdentifier(_out_buf)));

	 // Set Over The Air Activation parameters to be ready to join the LoRaWAN
	 printf("Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<\n", LORA_appEUI, LORA_appKEY, _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setOtaaIdentity(LORA_appEUI,LORA_appKEY,_out_buf)));

	 // Save all the MAC settings in the transceiver
	 printf("Save mac >%s<\n",lora_driver_mapReturnCodeToText(lora_driver_saveMac()));

	 // Enable Adaptive Data Rate
	 printf("Set Adaptive Data Rate: ON >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_setAdaptiveDataRate(LORA_ON)));

	 // Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
	 printf("Set Receiver Delay: %d ms >%s<\n", 500, lora_driver_mapReturnCodeToText(lora_driver_setReceiveDelay(500)));

	 // Join the LoRaWAN
	 uint8_t maxJoinTriesLeft = 10;
	 
	 do {
		 rc = lora_driver_join(LORA_OTAA);
		 printf("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

		 if ( rc != LORA_ACCEPTED){
			 // Make the red led pulse to tell something went wrong
			 status_leds_longPuls(led_ST1); // OPTIONAL
			 // Wait 5 sec and lets try again
			 vTaskDelay(pdMS_TO_TICKS(5000UL));
		 }
		 else
		 {
			 break;
		 }
	 } while (--maxJoinTriesLeft);

	 if ( rc == LORA_ACCEPTED)
	 {
		 // Connected to LoRaWAN :-)
		 // Make the green led steady
		 status_leds_ledOn(led_ST2); // OPTIONAL
	 }
	 else
	 {
		 // Something went wrong
		 // Turn off the green led
		 status_leds_ledOff(led_ST2); // OPTIONAL
		 // Make the red led blink fast to tell something went wrong
		 status_leds_fastBlink(led_ST1); // OPTIONAL

		 // Lets stay here
		 while (1)
		 {
			 taskYIELD();
		 }
	 }
}
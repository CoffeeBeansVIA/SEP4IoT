#include "lora_driver.h"
#include <message_buffer.h>
#include <status_leds.h>
#include "ATMEGA_FreeRTOS.h"
#include "SensorDataPackage.h"
#include "semphr.h"
#include "Configuration.h"

uint16_t maxHumSetting;
uint16_t maxTempSetting;
uint16_t maxCo2Setting;
MessageBufferHandle_t dl_messageBuffer;

void mutexPuts(char* str);
SemaphoreHandle_t DownLinkReceiveMutex;
SemaphoreHandle_t DownLinkUpdateMutex;

static void _lora_setup(void);

#define LORA_appEUI "926F9B5931FCA94C"
#define LORA_appKEY "1D2EB57B831FBDEF807978AE930786E4"

static char _out_buf[100];

MessageBufferHandle_t dl_messageBuffer;
static lora_driver_payload_t downlink_payload;

//lora_driver_initialise(ser_USART1, dl_messageBuffer);
void DL_handler_create(MessageBufferHandle_t _downlinkMessageBuffer )
{
	lora_driver_resetRn2483(1); // Activate reset line
	vTaskDelay(2);
	lora_driver_resetRn2483(0); // Release reset line
	vTaskDelay(150); 
	lora_driver_flushBuffers(); // get rid of first version string from module after reset!
	_lora_setup();
	dl_messageBuffer =_downlinkMessageBuffer;
}

void DL_handler_receive(void *pvParameters )
{
	for(;;)
	{
    xSemaphoreTake( DownLinkReceiveMutex , portMAX_DELAY); // Wait for the ReceiveMutex
	SensorDataPackage_t sensorDataPackage = SensorDataPackage_create();
	size_t xReceivedBytes;
	const TickType_t xBlockTime = pdMS_TO_TICKS( 20 );
	
	// Receive the next message from the message buffer.  Wait in the Blocked
	// state (so not using any CPU processing time) for a maximum of 100ms for
	// a message to become available.
	xReceivedBytes = xMessageBufferReceive( dl_messageBuffer,
	( void * ) sensorDataPackage,
	sizeof( SensorDataPackage_t ),
	xBlockTime );
	
	if( xReceivedBytes > 0 )
	{
		puts("DL_receive -> OK");
		puts(xReceivedBytes);
		
		// take the data out of the packet
		uint16_t co2_ppm = SensorDataPackage_getCO2(sensorDataPackage);
		
		// free up memory
		SensorDataPackage_free(sensorDataPackage);

        //update-start
		printf("DOWN LINK: from port: %d with %d bytes received!", downlink_payload.portNo, downlink_payload.len); // Just for debug
		if (2 == downlink_payload.len) // Check that we have got the expected 2 bytes
		{
			//decode the payload into our variables
			//maxHumSetting = (downlinkPayload.bytes[0] << 8) + downlinkPayload.bytes[1];
			//maxTempSetting = (downlinkPayload.bytes[2] << 8) + downlinkPayload.bytes[3];
			maxCo2Setting = (downlink_payload.bytes[0] << 8) + downlink_payload.bytes[1];
			
			//set new values
			//setTemperature(maxTempSetting);
			setCo2(maxCo2Setting);
			//setHumidity(maxHumSetting);
		}
        xSemaphoreGive( DownLinkReceiveMutex );
        xSemaphoreGive( DownLinkUpdateMutex );
		vTaskDelay(pdMS_TO_TICKS(300000));
	}
	else{
		 vTaskDelay(pdMS_TO_TICKS(150000));
	}
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

		if ( rc != LORA_ACCEPTED)
		{
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

	if (rc == LORA_ACCEPTED)
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
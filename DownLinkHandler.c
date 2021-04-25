#include <lora_driver.h>
#include <message_buffer.h>
#include "semphr.h"
#include "Configuration.c"
uint16_t maxHumSetting;
uint16_t maxTempSetting;
uint16_t maxCo2Setting;
lora_driver_payload_t downlinkPayload;

MessageBufferHandle_t dl_messageBuffer;


dl_messageBuffer = xMessageBufferCreate(sizeof(lora_driver_payload_t)*2);
lora_driver_initialise(ser_USART1, dl_messageBuffer);
void create(MessageBufferHandle_t xMessageBuffer)
{
	lora_driver_resetRn2483(1); // Activate reset line
	vTaskDelay(2);
	lora_driver_resetRn2483(0); // Release reset line
	vTaskDelay(150); // Wait for tranceiver module to wake up after reset
	lora_driver_flushBuffers(); // get rid of first version string from module after reset!
}
void DL_update(MessageBufferHandle_t xMessageBuffer){
	for (;;)
	{
		xMessageBufferReceive(downLinkMessageBufferHandle, &downlinkPayload, sizeof(lora_driver_payload_t), portMAX_DELAY);
		printf("DOWN LINK: from port: %d with %d bytes received!", downlinkPayload.portNo, downlinkPayload.len); // Just for debug
		if (9 == downlinkPayload.len) // Check that we have got the expected 9 bytes
		{
			// decode the payload into our variables
			maxHumSetting = (downlinkPayload.bytes[0] << 8) + downlinkPayload.bytes[1];
			maxTempSetting = (downlinkPayload.bytes[2] << 8) + downlinkPayload.bytes[3];
			maxCo2Setting = (downlinkPayload.bytes[4] << 8) + downlinkPayload.bytes[5];
			// set new values
			setTemperature(maxTempSetting);
			setCo2(maxCo2Setting);
			setHumidity(maxHumSetting);
		}
	}
}
void DL_receive( MessageBufferHandle_t xMessageBuffer )
{
	uint8_t ucRxData[ 20 ];
	size_t xReceivedBytes;
	const TickType_t xBlockTime = pdMS_TO_TICKS( 20 );
	
	// Receive the next message from the message buffer.  Wait in the Blocked
	// state (so not using any CPU processing time) for a maximum of 100ms for
	// a message to become available.
	xReceivedBytes = xMessageBufferReceive( xMessageBuffer,
	( void * ) ucRxData,
	sizeof( ucRxData ),
	xBlockTime );
	
	if( xReceivedBytes > 0 )
	{
		// A ucRxData contains a message that is xReceivedBytes long.  Process
		// the message here....
		DL_update(xMessageBuffer);
	}
}
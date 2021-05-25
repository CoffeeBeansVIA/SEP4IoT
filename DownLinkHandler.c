//----------------------------INCLUDES----------------------------
#include "DownLinkHandler.h"
//-----------------------------------------------------------------


//--------------------------FUNCTIONS--------------------------
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
		size,
		xBlockTime
		);
		
		if(xReceivedBytes>0)
		{	
			
			printf("DOWN LINK: from port: %d with %d bytes received!", _downlink_payload.portNo, _downlink_payload.len); // Just for debug
			if (4 == _downlink_payload.len) // Check that we have got the expected 4 bytes
			{
				//decode the payload into our variables
				maxCo2Setting = (_downlink_payload.bytes[0] << 8) + _downlink_payload.bytes[1];
				maxCO2Fluct = (_downlink_payload.bytes[2] << 8) + _downlink_payload.bytes[3];
				//maxHumSetting = (_downlink_payload.bytes[4] << 8) + _downlink_payload.bytes[5];
				//maxHumFluct = (_downlink_payload.bytes[6] << 8) + _downlink_payload.bytes[7];
				//maxTempSetting = (_downlink_payload.bytes[8] << 8) + _downlink_payload.bytes[9];
				//maxTempFluct = (_downlink_payload.bytes[10] << 8) + _downlink_payload.bytes[11];
				
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
	//-----------------------------------------------------------------




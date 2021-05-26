//----------------------------INCLUDES---------------------------
#include "upLinkHandler.h"
//-------------------------------------------------------------
static char _out_buf[100];

//--------------------------FUNCTIONS--------------------------
void UL_handler_create(){

	xTaskCreate(
	Welcome_task
	,  "Welcome Task"
	,  configMINIMAL_STACK_SIZE
	,  NULL
	,  5
	,  NULL );

	xTaskCreate(
	UL_receive_task
	,  "UpLink Handler Receive"
	,  configMINIMAL_STACK_SIZE
	,  NULL
	,  3
	,  NULL );
}

void Welcome_task(void *pvParams){
	taskENTER_CRITICAL();
	// Hardware reset of LoRaWAN transceiver
	lora_driver_resetRn2483(1);
	vTaskDelay(2);
	lora_driver_resetRn2483(0);
	// Give it a chance to wakeup
	vTaskDelay(150);

	lora_driver_flushBuffers(); // get rid of first version string from module after reset!

	_lora_setup();	
	
	taskEXIT_CRITICAL();
	
	//sending 'hardware started message'
	lora_driver_payload_t _welcome_payload;
				
	uint16_t val = 100;
	_welcome_payload.bytes[0] = val >> 8;
	_welcome_payload.bytes[1] = val & 0xFF;
	_welcome_payload.len = 2;
	_welcome_payload.portNo = 3;
				
	status_leds_shortPuls(led_ST4);  // OPTIONAL
			
	printf("Welcome Message >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, &_welcome_payload)));
	vTaskSuspend(NULL);
}

void UL_receive_task( void *pvParameters )
{	
	 for(;;){ 
		 xSemaphoreTake( UpLinkSendMutex , portMAX_DELAY);
		 
		 SensorDataPackage_t sensorDataPackage = SensorDataPackage_create();
		 
		 size_t xReceivedBytes;
		 const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );

		 // Receive next message from the UL message buffer. Wait for a maximum of 100ms for a message to become available.
		 xReceivedBytes = xMessageBufferReceive( // Does not work properly... Fuck it, will do it the other way for now.
		 UpLinkMessageBuffer,
		 &sensorDataPackage,
		 sizeof( SensorDataPackage_t ),
		 xBlockTime
		 );
		 
		uint16_t co2_ppm = getCO2();
		
		lora_driver_payload_t _uplink_payload;
		
		_uplink_payload.bytes[0] = co2_ppm >> 8;
		_uplink_payload.bytes[1] = co2_ppm & 0xFF;
		_uplink_payload.len = 2;
		_uplink_payload.portNo = 2;

		status_leds_shortPuls(led_ST4);  // OPTIONAL
		printf("Upload Message >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, &_uplink_payload)));
		
		xSemaphoreGive(UpLinkReceiveMutex);
	 }

 }
 
void _lora_setup(void){
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
 //-------------------------------------------------------------

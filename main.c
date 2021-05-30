/*------------------------INCLUDES------------------------*/
#include "global.h"
#include <hih8120.h>

// Miscellaneous includes
#include "window_controller.h"
#include "co2Sensor.h"
#include "upLinkHandler.h"
#include "DownLinkHandler.h"

/*-----------------------PROCEDURES----------------------*/
void create_semaphores(void){
	// Semaphores initialization
	if(NULL == sysInitMutex){
		sysInitMutex = xSemaphoreCreateMutex();
		xSemaphoreTake(sysInitMutex, portMAX_DELAY);
	}
	
	if(NULL == measureCo2Mutex){
		measureCo2Mutex = xSemaphoreCreateMutex();
		xSemaphoreTake(measureCo2Mutex, portMAX_DELAY);
	}
	
	if ( NULL == UpLinkSendMutex ){
		UpLinkSendMutex = xSemaphoreCreateMutex();
		xSemaphoreTake(UpLinkSendMutex, portMAX_DELAY);
	}
	
	if(NULL == UpLinkReceiveMutex){
		UpLinkReceiveMutex = xSemaphoreCreateMutex();
		xSemaphoreTake(UpLinkReceiveMutex, portMAX_DELAY);
	}
	/*if ( NULL == DownLinkReceiveUpdateMutex ){
		DownLinkReceiveMutex = xSemaphoreCreateMutex();
		xSemaphoreTake(DownLinkReceiveMutex, portMAX_DELAY);
	}
	*/
	
	if(NULL == putsMutex){
		putsMutex = xSemaphoreCreateMutex();
		xSemaphoreGive( putsMutex );
	}
	if(NULL == windowMutex){
		windowMutex = xSemaphoreCreateMutex();
		xSemaphoreTake( windowMutex,portMAX_DELAY);
	}
}

void create_event_groups(void){
	measureEventGroup = xEventGroupCreate();
	readyEventGroup = xEventGroupCreate();
}

void create_message_buffers(void){
	UpLinkMessageBuffer = xMessageBufferCreate(UpLinkSize);
	DownLinkMessageBuffer = xMessageBufferCreate(DownLinkSize);
}

void trigger_CO2_measurement_task( void *pvParameters ){
	
	for(;;)
	{
		xSemaphoreTake(windowMutex, portMAX_DELAY);
		xEventGroupClearBits(readyEventGroup,BIT_TASK_CO2_READY);
		xEventGroupSetBits(measureEventGroup,BIT_TASK_CO2_MEASURE);
		
		//wait for ready bits from sensors(later when there will be more sensors it will have to handle different situations(see class diagram video))
		xEventGroupWaitBits(
		readyEventGroup,
		BIT_TASK_CO2_READY,
		pdTRUE,
		pdTRUE,
		portMAX_DELAY);
		
		xEventGroupClearBits(measureEventGroup,BIT_TASK_CO2_MEASURE);
		char buf[63];
		sprintf(buf, "CO2 Measurement: %d", getCO2());
		mutexPuts(buf);
		xSemaphoreGive(measureCo2Mutex);
	}
}

void UL_handler_send( void *pvParameters )
{
	const TickType_t x100ms = pdMS_TO_TICKS( 100 );
	
	for(;;){
		xSemaphoreTake( measureCo2Mutex , portMAX_DELAY);
		size_t xBytesSent;
		// Payload
		SensorDataPackage_t sensorDataPackage = SensorDataPackage_create();
		
		SensorDataPackage_setCO2(sensorDataPackage,getCO2());/*JULIA PUT YOUR DATA HERE - CO2Sensor.getCO2()*/
		
		int size = sizeof( sensorDataPackage );
		
		// Send the payload to the message buffer, a maximum of 100ms to wait for enough space to be available in the message buffer.
		xBytesSent = xMessageBufferSend( UpLinkMessageBuffer, ( void * ) sensorDataPackage, size, x100ms );
		
		if( xBytesSent != size )
		{
			// The call to xMessageBufferSend() timed out before there was enough space in the buffer for the data to be written.
			SensorDataPackage_free(sensorDataPackage);
			// Wait 2.5 minutes to retry
			vTaskDelay(pdMS_TO_TICKS(150000));
		}else{
			// Try to receive
			const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
			
			SensorDataPackage_t receivedDataPackage = SensorDataPackage_create();
			
			int xReceivedBytes = xMessageBufferReceive( // Does not work properly... Will do it the other way for now.
			UpLinkMessageBuffer,
			&receivedDataPackage,
			size,
			xBlockTime
			);
			
			if(xReceivedBytes > 0){
				char buff [63];
				sprintf(buff, "UL_handler_send Co2 = (%d) -> OK", SensorDataPackage_getCO2(receivedDataPackage));
				mutexPuts(buff);
				SensorDataPackage_free(receivedDataPackage);
				xSemaphoreGive(UpLinkSendMutex);
			}
			SensorDataPackage_free(receivedDataPackage);
	
		}
	}
}

void create_tasks(void){
	xTaskCreate(
	trigger_CO2_measurement_task
	,  "Trigger CO2 Measurement Task"
	,  configMINIMAL_STACK_SIZE
	,  NULL
	,  3
	,  NULL );
	
	xTaskCreate(
	UL_handler_send
	,  "UpLink Handler Send"  // A name just for humans
	,  configMINIMAL_STACK_SIZE  // This stack size can be checked & adjusted by reading the Stack High water
	,  NULL  // Params
	,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );

}

/*-------------------------------------------------------*/

void initialiseSystem( void ){
	configuration = Configuration_create();
	
	create_semaphores();
	
	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	
	// Set output ports for LEDs used in the example
	DDRA |= _BV(DDA0) | _BV(DDA7);
	
	create_event_groups();
	
	create_message_buffers();

	// LoRaWAN initialization
	// Status LEDs driver
	status_leds_initialise(5); // Priority 5 for internal task
	
	// Initialize the LoRaWAN driver with down-link buffer
	lora_driver_initialise(1, DownLinkMessageBuffer);
	// Create LoRaWAN task and start it up with priority 3
	
	CO2_handler_create();
	create_tasks();
	UL_handler_create();
	DL_handler_create();
	rcServoTask_create();

	xSemaphoreGive(sysInitMutex);
}

/*---------------------------MAIN----------------------------*/

int main(void){
	
	initialiseSystem();
	
	xSemaphoreTake(sysInitMutex , portMAX_DELAY);
	mutexPuts("Program Started!!\n");
	
	xSemaphoreGive(windowMutex);
	
	vTaskStartScheduler(); // Initialize and run the freeRTOS scheduler.
	
	//Execution will never reach here.
}
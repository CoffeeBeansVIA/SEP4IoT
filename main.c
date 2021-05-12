/*------------------------INCLUDES------------------------*/
#include <stdio.h>
#include <avr/io.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio_driver.h>
#include <serial.h>
#include <hih8120.h>
#include <event_groups.h>
// Needed for LoRaWAN
#include <lora_driver.h>
#include <status_leds.h>

// MISC includes
#include "SensorDataPackage.h"

/*------------------------DEFINES------------------------*/
// Mutexes
SemaphoreHandle_t sysInitMutex;
SemaphoreHandle_t measureCo2Mutex;
SemaphoreHandle_t UpLinkSendMutex;
SemaphoreHandle_t UpLinkReceiveMutex;
SemaphoreHandle_t putsMutex;

// Event groups
EventGroupHandle_t measureEventGroup = NULL;
#define BIT_TASK_CO2_MEASURE (1<<0)
EventGroupHandle_t readyEventGroup = NULL;
#define BIT_TASK_CO2_READY (1<<1)

// MessageBuffers
const int UpLinkSize = sizeof(SensorDataPackage_t)*2;
const int DownLinkSize = sizeof(lora_driver_payload_t)*2;
MessageBufferHandle_t UpLinkMessageBuffer = NULL;
MessageBufferHandle_t DownLinkMessageBuffer = NULL;

// Tasks & functions
void trigger_CO2_measurement_task( void *pvParameters );
void UL_handler_send( void *pvParameters );
uint16_t getCO2();
void UL_handler_create(MessageBufferHandle_t _uplinkMessageBuffer );
void CO2_handler_create();

/*-----------------------PROCEDURES----------------------*/
void create_semaphores(void){
	// Semaphores initialization
	/*if(NULL == sysInitMutex){
		sysInitMutex = xSemaphoreCreateMutex();
		xSemaphoreGive(sysInitMutex);
	}*/
	
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
	
	if(NULL == putsMutex){
		putsMutex = xSemaphoreCreateMutex();
		xSemaphoreGive( putsMutex );
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

void mutexPuts(char* str){
	if(xSemaphoreTake(putsMutex, portMAX_DELAY) == pdTRUE){
		puts(str);
		xSemaphoreGive(putsMutex);
	}
}

void initialiseSystem( void ){

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
	
	create_tasks();
	UL_handler_create(&UpLinkMessageBuffer);
	CO2_handler_create();
	
	mutexPuts("Program Started!!\n");
	
	xSemaphoreGive(UpLinkReceiveMutex);
}

void trigger_CO2_measurement_task( void *pvParameters ){
	
	for(;;)
	{
		xSemaphoreTake(UpLinkReceiveMutex, portMAX_DELAY);
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
	for(;;){
		xSemaphoreTake( measureCo2Mutex , portMAX_DELAY);
		size_t xBytesSent;
		// Payload
		SensorDataPackage_t sensorDataPackage = SensorDataPackage_create();
		
		SensorDataPackage_setCO2(sensorDataPackage,getCO2());/*JULIA PUT YOUR DATA HERE - CO2Sensor.getCO2()*/
		
		const TickType_t x100ms = pdMS_TO_TICKS( 100 );
		
		// Send the payload to the message buffer, a maximum of 100ms to wait for enough space to be available in the message buffer.
		xBytesSent = xMessageBufferSend( UpLinkMessageBuffer, ( void * ) sensorDataPackage, sizeof( sensorDataPackage ), x100ms );
		
		if( xBytesSent != sizeof( sensorDataPackage ) )
		{
			// The call to xMessageBufferSend() timed out before there was enough space in the buffer for the data to be written.
			// Wait 2.5 minutes to retry
			SensorDataPackage_free(sensorDataPackage);
			vTaskDelay(pdMS_TO_TICKS(150000));
		}else{
			// OK
			mutexPuts("UL_handler_send -> OK");
			SensorDataPackage_free(sensorDataPackage);
			xSemaphoreGive(UpLinkSendMutex);
		}
	}
}

/*---------------------------MAIN----------------------------*/

int main(void){
	
	initialiseSystem();
	
	vTaskStartScheduler(); // Initialize and run the freeRTOS scheduler.
	//Execution will never reach here.
}
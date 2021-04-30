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

/*-----------------------------------------------------------*/

// Prototypes
void UL_handler_create( MessageBufferHandle_t _uplinkMessageBuffer );
void measureCo2task( void *pvParameters );
void CO2_handler_create();
void UL_handler_send( void *pvParameters );
void UL_handler_receive( void *pvParameters );

// define semaphores
SemaphoreHandle_t xTestSemaphore;
SemaphoreHandle_t measureCo2Mutex;
SemaphoreHandle_t UpLinkSendMutex;
SemaphoreHandle_t UpLinkReceiveMutex;
SemaphoreHandle_t putsMutex;

//Event groups
EventGroupHandle_t measureEventGroup = NULL;
#define BIT_TASK_CO2_MEASURE (1<<0)
EventGroupHandle_t readyEventGroup = NULL;
#define BIT_TASK_CO2_READY (1<<1)

// MessageBuffers
const int UpLinkSize = sizeof(SensorDataPackage_t)*2;
const int DownLinkSize = sizeof(lora_driver_payload_t)*2;
MessageBufferHandle_t UpLinkMessageBuffer = NULL;
MessageBufferHandle_t DownLinkMessageBuffer = NULL;

/*-----------------------------------------------------------*/

void mutexPuts(char* str){
	if(xSemaphoreTake(putsMutex, portMAX_DELAY) == pdTRUE){
		puts(str);
		xSemaphoreGive(putsMutex);
	}
}


void create_tasks_and_semaphores(void)
{
	// Semaphores initialization
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
	
	xTaskCreate(
	measureCo2task
	,  "Measure CO2"
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
	
	xTaskCreate(
	UL_handler_receive
	,  "UpLink Handler Receive" 
	,  configMINIMAL_STACK_SIZE
	,  NULL
	,  3 
	,  NULL );
	
}

/*-----------------------------------------------------------*/
void task1( void *pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500/portTICK_PERIOD_MS; // 500 ms

	// Initialize the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		mutexPuts("Task1"); // stdio functions are not reentrant - Should normally be protected by MUTEX
		PORTA ^= _BV(PA0);
	}
}
/*-----------------------------------------------------------*/

void measureCo2task( void *pvParameters )
{
	CO2_handler_create();
	for(;;)
	{
		xSemaphoreTake(UpLinkReceiveMutex, portMAX_DELAY);
		xEventGroupSetBits(measureEventGroup,BIT_TASK_CO2_MEASURE);
		
		//wait for ready bits from sensors(later when there will be more sensors it will have to handle different situations(see class diagram video))
		xEventGroupWaitBits(
		readyEventGroup,
		BIT_TASK_CO2_READY,
		pdTRUE,
		pdTRUE,
		portMAX_DELAY);
		
		char buf[10];
		sprintf(buf, "CO2 Measurement: %d", getCO2());
		mutexPuts(buf);
		xSemaphoreGive(measureCo2Mutex);
	}
}
/*-----------------------------------------------------------*/

void UL_handler_send( void *pvParameters )
{
	UL_handler_create(UpLinkMessageBuffer);
	
	for(;;){
		xSemaphoreTake( measureCo2Mutex , portMAX_DELAY);
		size_t xBytesSent;
		// Payload
		SensorDataPackage_t sensorDataPackage = SensorDataPackage_create(); 
		
		SensorDataPackage_setCO2(sensorDataPackage,250);/*JULIA PUT YOUR DATA HERE - CO2Sensor.getCO2()*/
		
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

/*-----------------------------------------------------------*/
void initialiseSystem()
{
	// Set output ports for LEDs used in the example
	DDRA |= _BV(DDA0) | _BV(DDA7);

	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	// Let's create some tasks
	create_tasks_and_semaphores();
	
	//Event groups
	measureEventGroup = xEventGroupCreate();
	readyEventGroup = xEventGroupCreate();
	
	//Message Buffers
	UpLinkMessageBuffer = xMessageBufferCreate(UpLinkSize);
	DownLinkMessageBuffer = xMessageBufferCreate(DownLinkSize);

	// LoRaWAN initialization
	// Status LEDs driver
	status_leds_initialise(5); // Priority 5 for internal task
	
	// Initialize the LoRaWAN driver with down-link buffer
	lora_driver_initialise(1, DownLinkMessageBuffer);
	// Create LoRaWAN task and start it up with priority 3
	
}

/*-----------------------------------------------------------*/
int main(void)
{
	initialiseSystem(); // Must be done as the very first thing!!
	printf("Program Started!!\n");
	xSemaphoreGive(UpLinkReceiveMutex);
	vTaskStartScheduler(); // Initialize and run the freeRTOS scheduler.
	//Execution will never reach here.
}


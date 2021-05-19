#include <stdio.h>
#include <avr/io.h>
#include "ATMEGA_FreeRTOS.h"
#include <task.h>
#include <stdio_driver.h>
#include "DownLinkHandler.h"
#include "window_controller.h"


/*void DL_handler( void *pvParameters );

void create_tasks(void){
	xTaskCreate(
	DL_handler
	,  "DownLink Handler"  // A name just for humans
	,  configMINIMAL_STACK_SIZE  // This stack size can be checked & adjusted by reading the Stack High water
	,  NULL  // Params
	,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
}*/

void  init(void){
	stdio_initialise(ser_USART0);
	DDRA |= _BV(DDA0) | _BV(DDA7);
	
	//DL_handler_create();
	rcServoTask_create();
}

void main(void){
	init();
	vTaskStartScheduler();
	puts("program finished \n");
}
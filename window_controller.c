#include "window_controller.h"
#include <stdbool.h>
#include <rc_servo.h>
#include <semphr.h>
#include "Configuration.h"
#include "co2Sensor.h"
SemaphoreHandle_t windowControllerMutex;

SemaphoreHandle_t UpLinkSendMutex;

void mutexPuts(char* str);

//constants
#define RC_SERVO_UP_POSITION 100
#define RC_SERVO_DOWN_POSITION -100

#define RC_SERVO_TASK_NAME "rcservo"
#define RC_SERVO_TASK_PRIORITY (configMAX_PRIORITIES - 3)


//private variables
static bool _IsClosed;

void rcServoTask(void *pvParameters)
{
	Configuration_t configuration = Configuration_create();
	
	 
	for (;;)
	{
		xSemaphoreTake(UpLinkSendMutex, portMAX_DELAY);
				
    if(getCO2()>Configuration_getCO2(configuration))
	{
		if (_IsClosed == false)
		{
			mutexPuts("Window is closing \n");
			rc_servo_setPosition(14, RC_SERVO_UP_POSITION);
			_IsClosed = true;
		}
	}
	else if (getCO2()<Configuration_getCO2(configuration))
	{
		if (_IsClosed == true)
		{
			mutexPuts("Window is opening \n");
			rc_servo_setPosition(14, RC_SERVO_DOWN_POSITION);
			_IsClosed = false;
		}
	}
	xSemaphoreGive(windowControllerMutex);	
		 //vTaskDelay(pdMS_TO_TICKS(5000));
		/*switch (_tmp)
				{
					case CLOSED:
					if (_IsClosed == false)
					{
						puts("It's down \n");
						rc_servo_setPosition(14, RC_SERVO_UP_POSITION);
						_IsClosed = true;
					}
					case OPENED:
					if (_IsClosed == true)
					{
						puts("It's up \n");
						rc_servo_setPosition(14, RC_SERVO_DOWN_POSITION);
						_IsClosed = false;
					}
					
		         }*/
		
	}
}

void rcServoTask_create()
{
	rc_servo_initialise();
	xTaskCreate(rcServoTask, RC_SERVO_TASK_NAME, configMINIMAL_STACK_SIZE, NULL, RC_SERVO_TASK_PRIORITY, NULL);
}
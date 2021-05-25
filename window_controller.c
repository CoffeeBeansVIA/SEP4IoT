//----------------------------INCLUDES----------------------------
#include "window_controller.h"
#include "co2Sensor.h"

//----------------------------FUNCTIONS----------------------------
void rcServoTask(void *pvParameters)
{
	rc_servo_initialise();
	
	Configuration_t configuration = Configuration_create();
	
	for (;;)
	{
	
	xSemaphoreTake(UpLinkReceiveMutex, portMAX_DELAY);
	
    if(getCO2()>Configuration_getCO2(configuration))
	{
		if (_IsClosed == false)
		{
			mutexPuts("Window is closing \n");
			rc_servo_setPosition(0, RC_SERVO_UP_POSITION);
			_IsClosed = true;
		}
	}
	if (getCO2()<=Configuration_getCO2(configuration))
	{
		if (_IsClosed == true)
		{
			mutexPuts("Window is opening \n");
			rc_servo_setPosition(0, RC_SERVO_DOWN_POSITION);
			_IsClosed = false;
		}
	}	 
	vTaskDelay(pdMS_TO_TICKS(60000));
	xSemaphoreGive(windowMutex);
	}
	
}

void rcServoTask_create()
{
	xTaskCreate(
	rcServoTask, 
	RC_SERVO_TASK_NAME, 
	configMINIMAL_STACK_SIZE, 
	NULL, 
	3, 
	NULL);
}
//-----------------------------------------------------------------
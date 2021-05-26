//----------------------------INCLUDES----------------------------
#include "humidityTemperatureSensor.h"
//-----------------------------------------------------------------


//----------------------------FUNCTIONS----------------------------
void temperatureHumiditySensor_create()
{

	//starting the drivers
	_setup_temperature_humidity_driver();


	xTaskCreate(
	HumidityTemperatureSensorTask,
	"Humidity Temperature Sensor",
	configMINIMAL_STACK_SIZE,
	NULL,
	3,
	NULL);
}

void _setup_temperature_humidity_driver()
{
	//create driver
	hih8120_driverReturnCode_t rc = hih8120_initialise();
	if (HIH8120_OK != rc)
	{
		exit(1);
	}
}

void HumidityTemperatureSensorTask(void *pvParameters)
{
	for (;;)
	{	
		xSemaphoreTake( measureCo2Mutex , portMAX_DELAY);
	
		//wait for the start measuring bit
		xEventGroupWaitBits(measureEventGroup,
		HUMIDITY_TEMPERATURE_MEASURE_BIT,
		pdTRUE,
		pdTRUE,
		portMAX_DELAY);

		if ( HIH8120_OK	!= hih8120_wakeup()){
			mutexPuts("Something went wrong in Temperature sensor");
		}

		//sensor needs minimum 50ms to start measuring after wake up
		vTaskDelay(100);

		hih8120_driverReturnCode_t result = hih8120_measure();

		//delay to fetch the results from the sensor
		vTaskDelay(100);

		//checking the result
		if (HIH8120_OK != result)
		{
			//10 trials to get the measurement from the sensor
			int count = 10;
			while ((HIH8120_TWI_BUSY == result) && count > 0)
			{
				result = hih8120_measure();

				// we need delay to fetch the results from the sensor
				vTaskDelay(20);
				count--;
			}
		}

		// measurements
		if (result == HIH8120_OK)
		{
			_lastTemperature = hih8120_getTemperature();
			_lastHumidity = hih8120_getHumidity();

			//setting the bit to true to signalize that the measurement was completed
			xEventGroupSetBits(readyEventGroup, HUMIDITY_TEMPERATURE_READY_BIT);
		}
	}
}

float humidityTemperatureSensor_getTemperature()
{
	return _lastTemperature;
}

float humidityTemperatureSensor_getHumidity()
{
	return _lastHumidity;
}
//-----------------------------------------------------------------

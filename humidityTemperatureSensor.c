//----------------------------INCLUDES----------------------------
#include "humidityTemperatureSensor.h"
//-----------------------------------------------------------------


//----------------------------FUNCTIONS----------------------------
static void inline _setup_temperature_humidity_driver()
{
	//create driver
	int result = hih8120_isReady(); //??????
	if (HIH8120_OK != result)
	{
		exit(EXIT_FAILURE);
	}
}

void HumidityTemperatureSensorTask(void *pvParameters)
{
	for (;;)
	{
		//wait for the start measuring bit 
		xEventGroupWaitBits(_eventGroupHandleMeasure,
		HUMIDITY_TEMPERATURE_MEASURE_BIT,
		pdTRUE,
		pdTRUE,
		portMAX_DELAY);

		hih8120_driverReturnCode_t result;

			result = hih8120_wakeup();

			//sensor needs minimum 50ms to start measuring after wake up
			vTaskDelay(100);

			result = hih8120_measure();

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
				xEventGroupSetBits(_eventGroupHandleNewData, HUMIDITY_TEMPERATURE_READY_BIT);
			}
	}
	vTaskDelete(_temperatureHumiditySensorTaskHandle);
}

void temperatureHumiditySensor_create(EventGroupHandle_t pvEventHandleMeasure,
EventGroupHandle_t pvEventHandleNewData, SemaphoreHandle_t pPrintfSemaphore)
{
	//setting variables
	_xPrintfSemaphore = pPrintfSemaphore;
	_eventGroupHandleMeasure = pvEventHandleMeasure;
	_eventGroupHandleNewData = pvEventHandleNewData;
	_lastTemperature = 0;
	_lastHumidity = 0;

	//starting the drivers
	_setup_temperature_humidity_driver();


	xTaskCreate(HumidityTemperatureSensorTask,
	"Humidity Temperature Sensor",
	configMINIMAL_STACK_SIZE,
	NULL,
	3,
	NULL);
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



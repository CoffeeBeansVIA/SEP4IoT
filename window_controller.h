#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <task.h>

/**
* movement by servo
*/
typedef enum Move
{
	CLOSED,
	OPENED
} rcServo_Move_t;

/**
* \brief Creates the rcServo driver.
*/
void rcServoTask_create();
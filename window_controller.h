#pragma once
#ifndef WINDOWCONTROLLER_H_
#define WINDOWCONTROLLER_H_
//----------------------------INCLUDES----------------------------
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <rc_servo.h>
#include <semphr.h>
#include "Configuration.h"
#include <ATMEGA_FreeRTOS.h>
#include <stdio_driver.h>
#include <stdio.h>
#include <avr/io.h>
//-----------------------------------------------------------------


//----------------------------MUTEXES----------------------------
SemaphoreHandle_t windowMutex;
SemaphoreHandle_t UpLinkReceiveMutex;
void mutexPuts(char* str);
//-----------------------------------------------------------------


//----------------------------CONSTANTS----------------------------
#define RC_SERVO_UP_POSITION 100
#define RC_SERVO_DOWN_POSITION -100
#define RC_SERVO_TASK_NAME "rcservo"
//-----------------------------------------------------------------


//-----------------------PRIVATE VARIABLES--------------------------
static bool _IsClosed;
//-----------------------------------------------------------------


//---------------------MOVEMENT BY SERVO---------------------------
typedef enum Move
{
	CLOSED,
	OPENED
} rcServo_Move_t;
//-----------------------------------------------------------------


//----------------------------FUNCTIONS----------------------------
void rcServoTask_create();
//-----------------------------------------------------------------


#endif
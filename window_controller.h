#pragma once
#ifndef WINDOWCONTROLLER_H_
#define WINDOWCONTROLLER_H_
//----------------------------INCLUDES----------------------------
#include "global.h"
#include <stdbool.h>
#include <rc_servo.h>
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
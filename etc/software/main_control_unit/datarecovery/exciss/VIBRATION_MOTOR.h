/***********************************************************************
* DESCRIPTION :
*
* 
* AUTHOR :    Shintaro Fujita
*
**/


#ifndef _VIBRATION_MOTOR_H_
#define _VIBRATION_MOTOR_H_

#include "exciss.h"

void VIBRATION_MOTOR__init();

uint8_t VIBRATION_MOTOR__read_status_register();

void VIBRATION_MOTOR__init_driver();

void VIBRATION_MOTOR__set_effect(uint8_t a_slot, uint8_t a_effect_id);

bool VIBRATION_MOTOR__setup(char * my_Config);

void VIBRATION_MOTOR__go();

void VIBRATION_MOTOR__stop();

void VIBRATION_MOTOR__test();

void VIBRATION_MOTOR__loop();

#endif
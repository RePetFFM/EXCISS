/***********************************************************************
* DESCRIPTION :
*
* 
* AUTHOR :    Shintaro Fujita
*
**/

#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include "LED_pwm.h"
#include "config_atmega_port_mapping.h"

void LIGHTS__init();

void LIGHTS__front_led_pwm(int a_pwm);

void LIGHTS__back_led_pwm(int a_pwm);

#endif
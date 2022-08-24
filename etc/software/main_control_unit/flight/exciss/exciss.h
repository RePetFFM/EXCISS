/***********************************************************************
* FILENAME :        exciss.h                         DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Power managment testing and avaluation software.
* Follwoing hardware components are controlled by this library:
* - Charger Texas Instruments bq25700A
* - Battery pack manager and gauge from Texas Instruments BQ40Z50-R2
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/

#ifndef _EXCISS_H_
#define _EXCISS_H_

#include <avr/wdt.h>
// #include <Wire.h>


#include "config.h"
#include "config_scu.h"
#include "config_atmega_port_mapping.h"
#include "config_statemachine_definitions.h"
#include "config_hvu.h"
#include "config_TimeKeeper.h"

// include device services
#include "BusMaster.h"
#include "POWER.h"
#include "TimeKeeper.h"
#include "USBSTORAGE.h"
#include "SCU.h"
#include "VIBRATION_MOTOR.h" 
#include "COM.h"
#include "LIGHTS.h"
#include "HVU.h"
#include "TEMPSENS.h"
#include "NONVOLATILEMEM.h"

// declerations for services
extern BusMaster busMaster;

extern COM com;

extern uint8_t _c_boolean_true;
extern uint8_t _c_boolean_false;

void MCU__set_science_scu_keep_alive(unsigned long aSeconds);

void MCU__set_science_scu_wakup(unsigned long aSeconds);

void MCU__log_timer_status();

#define COMP_DATE __DATE__
#define COMP_TIME __TIME__

#endif	// _EXCISS_H_
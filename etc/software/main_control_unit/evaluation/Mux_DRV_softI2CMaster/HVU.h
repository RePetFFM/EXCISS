/***********************************************************************
* FILENAME :        hvu.h             DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
*       library for high voltage unit periferial control.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    18 Feb 2018
*
**/


/*
uint8_t SM_HVU__state;

int HVU__last_target_voltage;
int HVU__last_actual_voltage;
int HVU__last_after_ignition_voltage;
int HVU__requested_ignition_voltage = 0;
*/

#ifndef _HVU_H_
#define _HVU_H_

#include "config_hvu.h"



void    HVU__init();

void    HVU__loop();

void    HVU__enable();

void    HVU__disable();

int     HVU__get_charge();

void    HVU__set_ignition_delta(int a_voltage);

void    HVU__logout_status();

bool    HVU__has_enough_charge();

void    HVU__stop_charging();

void    HVU__abort();

void    HVU__set_charg_target(int a_voltage);

void    HVU__start_charging(int a_voltage);

void    HVU__ignite();

void    HVU__do_test_ignition_without_arc();

void    HVU__ignite_at_charg(int a_voltage);


#endif
/***********************************************************************
*
* DESCRIPTION :
* AUTHOR :    Shintaro Fujita
*
**/

#ifndef _POWER_H_
#define _POWER_H_

#include "PSU.h" 


void POWER__init();


void POWER__loop();

void POWER__loop_check_charg_ok();

void POWER__loop_battery_level_check();


// getter for power status
uint8_t POWER__charg_ok();

uint8_t POWER_battery_enough_power_for_scu_start();

uint8_t POWER_battery_enough_power_for_scu_run();

uint8_t POWER__mode();


// power modes
void POWER__mode_minimal();


// scu power methods
void POWER__mode_scu_bootup_prepare();

void POWER__mode_scu_bootup();


// science power methods
uint8_t POWER__science_power_enabled();

void POWER__mode_start_science_experiment();

void POWER__mode_end_science_experiment();

void POWER__enable_vibration_motor();


// logout methods
void POWER__logout_charger();

void POWER__logout_gauge();

void POWER__logout_power_switches();



#endif	// _POWER_H_


/***********************************************************************
* DESCRIPTION :
*
* 
* AUTHOR :    Shintaro Fujita
*
**/


#ifndef _TIMEKEEPER_H_
#define _TIMEKEEPER_H_

#include "exciss.h"

void TIMEKEEPER__init();

void TIMEKEEPER__healthcheck();

void TIMEKEEPER__log_current_datetime();

String TIMEKEEPER__get_DateTime_String();

void TIMEKEEPER__set_time(String new_Time, bool timeRecoverMode);

uint8_t TIMEKEEPER__check_alert_event();

uint32_t TIMEKEEPER__timer_remaining_time(uint8_t aEventID);

uint32_t TIMEKEEPER__delta_to_next_datatransfer_seconds();

uint8_t TIMEKEEPER__ok_to_bootup_scu();

void TIMEKEEPER__enable_datatransfer_window();

void TIMEKEEPER__disable_datatransfer_window();

uint8_t TIMEKEEPER__datatransfer_window_enable_yes();

uint8_t TIMEKEEPER__datatransfer_window_enable_no();

void TIMEKEEPER__logout_datatransfer_window_enable_flag();


struct TIMEKEEPER__ALERT {
	bool enabled;
	unsigned long interval; 			// if it is not a interval timer, set interval value to 0UL
	unsigned long timestamp;
	uint8_t eventID;
};


#endif
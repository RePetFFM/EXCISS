/***********************************************************************
* DESCRIPTION :
*
* 
* AUTHOR :    Shintaro Fujita
*
**/


#ifndef _CONFIG_SCU_H_
#define _CONFIG_SCU_H_

// ------------------------------------
// timekeeper
//
// all values in seconds as unsigned long

// turns off unused peripherials 1min and switch usb_x to ISS
#define SCU__DELAY_BOOTDOWN_FINALISE						    90UL

// default value 10min ( 60UL * 10UL )
#define SCU__ENTER_RECOVERYMODE_AFTER_POWERUP                   60UL * 10UL        

// default 5 min (60UL * 5UL)
#define SCU__DEFAULT_SHUTDOWN_DELAY                             60UL * 5UL

// default 10min 60UL * 10UL
#define SCU__DEFAULT_WAKEUP_INTERVAL							60UL * 10UL

#endif
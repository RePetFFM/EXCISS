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

#include <Arduino.h>
#include "SCU.h"
#include "config_scu.h"
#include "config_atmega_port_mapping.h"
#include "exciss.h"



uint32_t _scu__boot_mode; // UPDATE or SCIENCE

uint32_t _scu__boot_mode_requested;

uint8_t _scu__boot_successful = 0x55;

uint8_t _scu__sd_card = 0x55;



void SCU__init() {
	pinMode( DOUT_SCU_SD_TOGGLE , OUTPUT );
	digitalWrite( DOUT_SCU_SD_TOGGLE, STATE_SCU_SD_TOGGLE_XU1100 );

	pinMode( DOUT_SCU_SHUTDOWN_REQUEST , OUTPUT );
	digitalWrite( DOUT_SCU_SHUTDOWN_REQUEST, STATE_SCU_RUN );

	_scu__boot_mode_requested = SCU__BOOT_MODE_RECOVERY ;  // first bootup in recovery mode after power up 

	_scu__boot_mode = 0;

}

	
void SCU__set_next_bootup_mode(uint32_t a_boot_mode) {
	_scu__boot_mode_requested = a_boot_mode;
}


void SCU__request_bootup() {
	switch(_scu__boot_mode_requested) {
		case SCU__BOOT_MODE_RECOVERY:

			_scu__boot_mode = _scu__boot_mode_requested;

			_scu__boot_mode_requested = SCU__BOOT_MODE_SCIENCE; // default boot mode after recovery mode is science mode

			SCU__state_bootup();
		break;

		case SCU__BOOT_MODE_SCIENCE:
			_scu__boot_mode = _scu__boot_mode_requested;

			SCU__state_bootup();
		break;		

		default:
		break;
	}
}


void SCU__logout_current_mode() {
	_scu__boot_successful = 0xAA;

	com.log("[ SCU ]");
	com.newline();
	com.log("scu_mode = ");

	switch(_scu__boot_mode) {
		case SCU__BOOT_MODE_RECOVERY:
			com.log("RECOVERY");
		break;

		case SCU__BOOT_MODE_SCIENCE:
			com.log("SCIENCE");
		break;		

		default:
			com.log("error: mode not selected");
		break;
	}


	// get current selected sd card slot
	com.newline();
	com.log("sd_card = ");	

	if( _scu__sd_card == 0x55 ) {
		com.log("1");	
	}
	
	if( _scu__sd_card == 0xAA ) {
		com.log("2");	
	}


	// get current charg_ok status
	uint8_t charg_ok = POWER__charg_ok();
	com.newline();
	com.log("frame_power = ");	
	charg_ok == 0xAA ? com.log("1") : com.log("0");
	com.newline();
}


void SCU__state_bootup() {
	_scu__boot_successful = 0x55; // resset boot successful flag to not
	// define sd card
	// digitalWrite( DOUT_SCU_SD_TOGGLE, STATE_SCU_SD_TOGGLE_XU1100 );

	digitalWrite( DOUT_SCU_SHUTDOWN_REQUEST, STATE_SCU_RUN );

	// TIMEKEEPER__timer_minimum_power_mode( 0UL ); // disable minimum power timer


	POWER__mode_scu_bootup_prepare();

	delay(500);

	USBSTORAGE__hub_reset();

	delay(500); // give some time to resset the usb hub

	USBSTORAGE__x_to_scu();

	SCU__sd_card_switching();


	delay(100);
	

	POWER__mode_scu_bootup();

	// POWER__mode_start_science_experiment(); // TODO: REMOVE BEFORE FLIGHT

	// TIMEKEEPER__timer_scu_shutdown( SCU__DEFAULT_SHUTDOWN_DELAY );

	com.log("[ SCU ]");
	com.newline();
	com.log("SCU__state_bootup");
	com.newline();
}


void SCU__request_shutdown() {
	// togle scu shutdown gpio pin for requesting shutingdown the scu
	digitalWrite( DOUT_SCU_SHUTDOWN_REQUEST, STATE_SCU_SHUTDOWN_REQUEST );

	// shutdown following power pathes:
	// 12v
	// VSYS
	// LED's 
	POWER__mode_end_science_experiment();

}


void SCU__sd_card_switching() {
	com.log("[ SCU ]");
	com.newline();
	com.log("sd_card = ");	

	if( _scu__sd_card == 0x55 ) {
		com.log("1");	
	}
	
	if( _scu__sd_card == 0xAA ) {
		com.log("2");	
	}
	
	com.newline();
	
	if( _scu__sd_card == 0x55 ) {
		digitalWrite( DOUT_SCU_SD_TOGGLE, STATE_SCU_SD_TOGGLE_XU1100 );
	}
	
	if( _scu__sd_card == 0xAA ) {
		digitalWrite( DOUT_SCU_SD_TOGGLE, STATE_SCU_SD_TOGGLE_XU1200 );
	}

	SCU__logout_sd_card_active();
}


void SCU__logout_sd_card_active() {
	com.log("[ SCU ]");
	com.newline();
	com.log("sd_card = ");	

	if( _scu__sd_card == 0x55 ) {
		com.log("1");	
	}
	
	if( _scu__sd_card == 0xAA ) {
		com.log("2");	
	}
	
	com.newline();
}


void SCU__bootdown_finalise() {
	com.log("[ SCU ]");
	com.newline();
	com.log("scu bootdown finalise");	
	com.newline();
	
	if( _scu__boot_successful == 0x55 ) {
		// scu dosn't requestes the mode, a bootup issue is assumed
		// do a SD-Card swap

		// swap active SD-card slot
		_scu__sd_card == 0x55 ? _scu__sd_card = 0xAA : _scu__sd_card = 0x55;
		com.log("sd card swap required");	
		com.newline();
	}

	if( _scu__boot_successful == 0xAA ) {
		// scu does boot
		// do nothing, everything is ok
	}

	
	com.log("sd_card = ");	
	if( _scu__sd_card == 0x55 ) {
		com.log("1");	
	}
	
	if( _scu__sd_card == 0xAA ) {
		com.log("2");	
	}
	com.newline();	

	USBSTORAGE__x_to_iss();
}


void SCU__sd_card_to_XU1100() {
	com.log("[ SCU ]");
	com.newline();
	com.log("sd_card = 1");	
	com.newline();
	
	_scu__sd_card = 0x55;
}


void SCU__sd_card_to_XU1200() {
	com.log("[ SCU ]");
	com.newline();
	com.log("sd_card = 2");	
	com.newline();
	
	_scu__sd_card = 0xAA;	
}


void SCU__loop() {

	
}
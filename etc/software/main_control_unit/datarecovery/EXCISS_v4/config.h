/***********************************************************************
* FILENAME :        config.h             DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
*       basic configuration with basic parameters
*
* NOTES :
*       This config is part of the EXCISS_v4.ino.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    18 Feb 2018
*
**/


// ------------------------------------
// UART config
#define UART_BAUTRATE   9600



// ------------------------------------
// HVU config
#define HVU_VOLTAGE_MAX_CHARGE				653		// max volt <==> analogRead max value (1024)
#define HVU_IGNITE_TOGGLE_DURATION			10 		// milliseconds
#define HVU_MEASURE_WAIT_AFTER_IGNITION	 	100 	// milliseconds
#define HVU_DISCHARGE_SUCCESS_THRESHOLD		150		// volt

#define HVU_MIN_CHARGE_VALUE		150		// volt
#define HVU_MAX_CHARGE_VALUE		600		// volt
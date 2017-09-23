
#include <Wire.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include "RTClib.h"
#include "exciss.h"
#include "Adafruit_DRV2605.h"
#include "powermanager.h"
#include "chargemonitor.h"


// RTC
RTC_DS3231 rtc;


// Globals for DRV2605_Haptic

Adafruit_DRV2605 DRV2605__shaker;
String DRV2605__effect_sequence = "64_64_64_64_64_64_64";
uint8_t DRV2605__shaker_run = 0;




void setup() {
	wdt_enable(WDTO_8S); // set watchdog interval to 8 seconds

	
	CORE__init_pins();
	

	wdt_reset(); // reset watchdog timer
}

// global states
uint32_t CORE__init_done = CORE__INIT_EXECUTE; // 0xA5A5 = init run state, 0x5A5A = init done


// init state machine
uint32_t CORE__init_state = CORE__INIT_STATE_SET_PWM_FREQ; // the state char are mirrord 4bit values. For example the 4bit value 0101 
char init_i2c_failure_codes[3];// failure code variales
char init_failure_code = 0;

// main state machine
uint32_t CORE__main_state = CORE__MAIN_SM_T_RECOVER; // the state char are mirrord 4bit values. For example the 4bit value 0101 
uint8_t CORE__statemachine_retry = 0;

// powermanagment state machine
uint32_t CORE__powermanagment_state = CORE__POWER_SM_T_INIT; // the state char are mirrord 4bit values. For example the 4bit value 0101 

// highvoltage state machine
uint32_t CORE__ignition_state = CORE__IGNITION_SM_T_INIT;

char ignition_armed = 0x5A; // 0xA5 = armed, 0x5A = disarmed
int ignition_charging_capacity_target_voltage = 0; // must be configured via serial from raspberry pi.
char ignition_failure_code = 0;  // 00 - FF





void loop() {
	if(CORE__init_done == CORE__INIT_EXECUTE) {
		CORE__statemachine_init();
	} 

	if(CORE__init_done == CORE__INIT_DONE) {
		powermanager_poll();

		SERIAL__Parser();

		CORE__statemachine_main();

		CORE__statemachine_powermanagment();

		CORE__statemachine_ignition();

		DRV2605__shaker_loop();
	}

	wdt_reset(); // reset watchdog timer
}

//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: Core

void CORE__init_pins() {
	// left side
	pinMode(CORE__PIN_DIN_RASPI_WATCHDOG, INPUT);
	pinMode(CORE__PIN_DOUT_MOSFET_5V, OUTPUT);
	pinMode(CORE__PIN_DOUT_MOSFET_8V, OUTPUT);
	pinMode(CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER, OUTPUT);
	pinMode(CORE__PIN_DOUT_SCIENCE_ARC_CHARG, OUTPUT);
	pinMode(CORE__PIN_PWM_POWERLED_FRONT, OUTPUT);

	// right side
	pinMode(CORE__PIN_DOUT_USB_SWITCH_SWITCH, OUTPUT);
	pinMode(CORE__PIN_DOUT_BABYSITTER_SYSOFF, OUTPUT);
	pinMode(CORE__PIN_PWM_POWERLED_BACK, OUTPUT);

	// initial pin state
	digitalWrite(CORE__PIN_DOUT_MOSFET_5V, LOW);
	digitalWrite(CORE__PIN_DOUT_MOSFET_8V, LOW);
	digitalWrite(CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER, LOW);
	digitalWrite(CORE__PIN_DOUT_SCIENCE_ARC_CHARG, LOW);
	POWERLED_frontlight(0);

	digitalWrite(CORE__PIN_DOUT_USB_SWITCH_SWITCH, USBSWITCH__SWITCH_TRANSFER);
	digitalWrite(CORE__PIN_DOUT_BABYSITTER_SYSOFF, HIGH);
	POWERLED_backlight(0);
}


void CORE__init_value_after_powerup() {
	DRV2605__init_if_required();
}

void CORE__init_i2c_components() {
	if (! rtc.begin()) {
		Serial.println("ERROR Couldn't find RTC");
	} else {
		Serial.println("rtc ready");
	}
}

char CORE__eeprom_read_char(int address) {
	return EEPROM.read(address);
}

void CORE__eeprom_write_char(int address,char value) {
	EEPROM.update(address,value);
}

void CORE_statemachine_state_index_validator() {
	// todo validate 

	// CORE__init_state
	// CORE__main_state
	// CORE__statemachine_powermanagment
}

void CORE__statemachine_init() {
	CORE_statemachine_state_index_validator(); // prevent executing wrong code due to corrupted state index

	switch (CORE__init_state) {
		case CORE__INIT_STATE_SET_PWM_FREQ:
			// set higher pwm frequency for the leds to prevent flicker in video recording
			TCCR1B = TCCR1B & 0b11111000 | 0x01; // 0x01 = 31250Hz/1, 0x02 = 31250Hz/8


			CORE__init_state = CORE__INIT_STATE_I2C_BABYSITTER;
			CORE__statemachine_retry = 0;
		break;

		case CORE__INIT_STATE_I2C_BABYSITTER:
			if (!powermanager_begin() && (CORE__statemachine_retry<10)) {
				// retry if init was not successful
				CORE__statemachine_retry++;
				init_i2c_failure_codes[0] = 0xA0;
				init_i2c_failure_codes[0] &= CORE__statemachine_retry;
				delay(10);
			} else if(CORE__statemachine_retry>=10) {
				init_i2c_failure_codes[0] = 0xF1; // major failor babysitter doesen't responde after 10 try's
				CORE__init_state = CORE__INIT_STATE_I2C_RTC;
				CORE__statemachine_retry = 0;
			} else {
				CORE__init_state = CORE__INIT_STATE_I2C_RTC;
				CORE__statemachine_retry = 0;	
			}
		break;

		case CORE__INIT_STATE_I2C_RTC:
			if (!rtc.begin() && (CORE__statemachine_retry<10)) {
				// retry if init was not successful
				CORE__statemachine_retry++;
				init_i2c_failure_codes[1] = 0xA0;
				init_i2c_failure_codes[1] &= CORE__statemachine_retry;
				delay(10);
			} else if(CORE__statemachine_retry>=10) {
				init_i2c_failure_codes[1] = 0xF1; // major failor babysitter doesen't responde after 10 try's
				CORE__init_state = CORE__INIT_STATE_I2C_RTC;
				CORE__statemachine_retry = 0;
			} else {
				CORE__statemachine_retry = 0;
				CORE__init_state = CORE__INIT_STATE_INIT_SERIAL;
			}
		break;

		case CORE__INIT_STATE_INIT_SERIAL:
			Serial.begin( UART_BAUTRATE );
			CORE__init_state 	= CORE__INIT_STATE_END;
			CORE__init_done 	= CORE__INIT_DONE;
		break;
	}
}

void CORE__statemachine_main() {
	static unsigned long main_statemachine_delay = 0;
	CORE_statemachine_state_index_validator();

	switch (CORE__main_state) {
		case CORE__MAIN_SM_T_RECOVER:
			CORE__main_state = CORE__MAIN_SM_L_RECOVER;
		break;

		case CORE__MAIN_SM_L_RECOVER:
			CORE__main_state = CORE__MAIN_SM_T_IDLE;
		break;

		case CORE__MAIN_SM_T_IDLE:
			CORE__main_state = CORE__MAIN_SM_L_IDLE;
		break;

		case CORE__MAIN_SM_L_IDLE:
			
			switch(TIMEKEEPER__get_current_timeframe()) {
				case TIMEKEEPER__DATA_TRANSFER_WINDOW:
					CORE__main_state = CORE__MAIN_SM_T_DATATRANSFER_MODE;
				break;

				case TIMEKEEPER__SCIENCE_WINDOW:
					CORE__main_state = CORE__MAIN_SM_T_SCIENCE_GO_CONDITION_CHECK;
				break;
			}
		break;

		case CORE__MAIN_SM_T_DATATRANSFER_MODE:
			USB_DATASWITCH_try_switch_to_transfer_mode();
			CORE__main_state = CORE__MAIN_SM_L_DATATRANSFER_MODE;
		break;

		case CORE__MAIN_SM_L_DATATRANSFER_MODE:
			if(TIMEKEEPER__get_current_timeframe()==TIMEKEEPER__SCIENCE_WINDOW) {
				CORE__main_state = CORE__MAIN_SM_T_DATATRANSFER_MODE_CLOSE_DELAY;
				main_statemachine_delay = millis()+100;
			}
		break;

		case CORE__MAIN_SM_T_DATATRANSFER_MODE_CLOSE_DELAY:
			if(millis()>=main_statemachine_delay) {
				CORE__ignition_state = CORE__MAIN_SM_L_IDLE;
			}
		break;
		
		case CORE__MAIN_SM_T_SCIENCE_GO_CONDITION_CHECK:
			if(powermanager_has_power_for_experiment()) {
				// TODO!!! Check for minimum execution time until transfer time frame 
				CORE__main_state = CORE__MAIN_SM_T_SCIENCE_GO;
			}

			CORE__main_state = CORE__MAIN_SM_L_IDLE;			
		break;

		case CORE__MAIN_SM_T_SCIENCE_GO:
			// switch USB to raspberry pi
			// flush serial buffer and enable serial parser
			USB_DATASWITCH_try_switch_to_science_mode();
			powermanager_8V_on();
			main_statemachine_delay = millis()+100; // wait for 100 millis befor turn on raspberry pi
			CORE__main_state = CORE__MAIN_SM_T_SCIENCE_GO_RASPI_POWERUP;
		break;

		case CORE__MAIN_SM_T_SCIENCE_GO_DELAY:
			if(millis()>=main_statemachine_delay) {
				CORE__ignition_state = CORE__MAIN_SM_T_SCIENCE_GO_RASPI_POWERUP;
			}
		break;

		case CORE__MAIN_SM_T_SCIENCE_GO_RASPI_POWERUP:
			// powering up 5V switched power rail. The raspberry pi will begin with booting
			powermanager_5V_on();
			raspberry_state = RASPBERRY__state_booting;
			CORE__main_state = CORE__MAIN_SM_L_SCIEMCE_WAIT_RASPI_BOOT;
		break;

		case CORE__MAIN_SM_L_SCIEMCE_WAIT_RASPI_BOOT:
			// waiting for first raspberry pi bootup done
			// wait boot done for ###sec. if boot takes too long shutdown 5V power source
			if(raspberry_state == RASPBERRY__state_operational) {
				CORE__main_state = CORE__MAIN_SM_T_SCIENCE_RASPI_BOOT_DONE;	
			}
		break;

		case CORE__MAIN_SM_T_SCIENCE_RASPI_BOOT_DONE:
			// release ignition statemachine
			
			CORE__main_state = CORE__MAIN_SM_L_SCIENCE_MISSION_ABORT_OR_DONE;
		break;

		case CORE__MAIN_SM_L_SCIENCE_MISSION_ABORT_OR_DONE:
			// check for science abort condition
			// abort conditions are:
			// - power state is below science minimum power requirement for over 10sec
			// - ignition state machine reports unrecaverable fault
			// - raspberry pi requests shutdown 

			// waiting for raspberry pi shutdown request
			if(raspberry_state != RASPBERRY__state_critical) {
				raspberry_state = RASPBERRY__state_shutdown;	
				CORE__main_state = CORE__MAIN_SM_T_SCIENCE_STOP;
			}
			
			
		break;

		case CORE__MAIN_SM_T_SCIENCE_ABORT:
			powermanager_8V_off();
		break;

		case CORE__MAIN_SM_T_SCIENCE_STOP:
			// if ### second over expected raspberry pi shutdown request, try a gracefull shutdown.
			// if no "shutting down" message is recieved for 5 second, turn off 5V power source for the Raspberry pi.
			CORE__main_state = CORE__MAIN_SM_T_SCIENCE_SHUTDOWN;
		break;

		case CORE__MAIN_SM_T_SCIENCE_SHUTDOWN:
			// turn off all component not required for idle mode
			powermanager_5V_off();
			raspberry_state = RASPBERRY__state_offline;
			CORE__main_state = CORE__MAIN_SM_L_IDLE;
		break;
	}

}

void CORE__statemachine_powermanagment() {
	CORE_statemachine_state_index_validator(); // prevent executing wrong code due to corrupted state index

	switch (CORE__powermanagment_state) {
		case CORE__POWER_SM_T_INIT:
			CORE__powermanagment_state = CORE__POWER_SM_T_HIBERNATE_MODE;
		break;

		case CORE__POWER_SM_T_HIBERNATE_MODE:
			CORE__powermanagment_state = CORE__POWER_SM_L_HIBERNATE_MODE;
		break;

		case CORE__POWER_SM_L_HIBERNATE_MODE:
			// check usb state
			// check battery state
			CORE__powermanagment_state = CORE__POWER_SM_T_SLEEP_MODE;
		break;

		case CORE__POWER_SM_T_SLEEP_MODE:
			CORE__powermanagment_state = CORE__POWER_SM_L_SLEEP_MODE;
		break;

		case CORE__POWER_SM_L_SLEEP_MODE:
			// transition to to deep sleep mode is required if any of the following condition is true:
			// usb power supply is more than # days not available
			// deep sleep mode was requested from raspberry pi
			// deep sleep mode was requested by usb power cycle command
			// goto deep sleep mode due to mission end
			CORE__powermanagment_state = CORE__POWER_SM_L_SCIENCE_POWER_READY;
		break;

		case CORE__POWER_SM_L_SCIENCE_POWER_READY:
			CORE__powermanagment_state = CORE__POWER_SM_T_SCIENCE_POWER_OK;
		break;

		case CORE__POWER_SM_T_SCIENCE_POWER_OK:
			CORE__powermanagment_state = CORE__POWER_SM_L_SCIENCE_POWER_MODE;
		break;

		case CORE__POWER_SM_L_SCIENCE_POWER_MODE:
			// if usb power is down and science mission is running, following conditions must be all true, to be able to continue with the experiment.
			// is battery charglevel above minimum science power requirement?	
			CORE__powermanagment_state = CORE__POWER_SM_T_SLEEP_MODE;
		break;
	}
	
}

uint8_t ignition_fail_counter = 0;

int ignition_requested_charging_voltage = 0;

void CORE__statemachine_ignition() {
	static unsigned long ignition_max_charging_runtime; 
	static unsigned long ignition_retry_delay;

	CORE_statemachine_state_index_validator(); // prevent executing wrong code due to corrupted state index
	
	switch (CORE__ignition_state) {

		case CORE__IGNITION_SM_T_INIT:
			CORE__ignition_state = CORE__IGNITION_SM_L_OFF;

			ignition_armed = 0x5A;
			ignition_requested_charging_voltage = 0;
			ignition_charging_capacity_target_voltage = 0;
		break;

		case CORE__IGNITION_SM_L_OFF:
			if(ignition_armed==0xA5) {
				ignition_requested_charging_voltage = 0;
				ignition_charging_capacity_target_voltage = 0;
				CORE__ignition_state = CORE__IGNITION_SM_L_IDLE;
			}
		break;

		case CORE__IGNITION_SM_L_IDLE:
			// wait state for charg reques from raspberry pi
			if(ignition_requested_charging_voltage>=CORE_IGNITION_MIN_CHARG_VOLTAGE) {
				ignition_charging_capacity_target_voltage = ignition_requested_charging_voltage;
				CORE__ignition_state = CORE__IGNITION_SM_T_CHARGE;
			} else if(ignition_requested_charging_voltage==-1) {
				ignition_charging_capacity_target_voltage = 0;
				ignition_failure_code = 1;
				CORE__ignition_state = CORE__IGNITION_SM_T_ABORT_DUE_FAILURE;
			}
		
		break;

		case CORE__IGNITION_SM_T_CHARGE:
			if(ignition_charging_capacity_target_voltage>0) {
				chargemonitor_start_charging(ignition_charging_capacity_target_voltage);	
				ignition_max_charging_runtime = millis() + 300000L; // 300 second max charging time
				CORE__ignition_state = CORE__IGNITION_SM_L_CHARGE;
			} else {
				// if cap charg voltage target is 0 (init default value) abort ignition
				// send failure code to raspberry pi
				chargemonitor_abort();
				ignition_failure_code = 11;
				CORE__ignition_state = CORE__IGNITION_SM_T_ABORT_DUE_FAILURE;
			}
			
		break;

		case CORE__IGNITION_SM_L_CHARGE:
			// charging arc capacitor until requested voltage value
			if(chargemonitor_has_enough_charge()) {
				chargemonitor_stop_charging();
				CORE__ignition_state = CORE__IGNITION_SM_T_IGNITION_READY;
			}

			if(millis()>=ignition_max_charging_runtime) {
				// max chargingtime was exceeded abort charging
				chargemonitor_abort();
				ignition_failure_code = 12;
				CORE__ignition_state = CORE__IGNITION_SM_T_ABORT_DUE_FAILURE;
			}
			
		break;

		case CORE__IGNITION_SM_T_IGNITION_READY:
			ignition_fail_counter = 0;
			CORE__ignition_state = CORE__IGNITION_SM_L_IGNITION_WAIT;
		break;

		case CORE__IGNITION_SM_L_IGNITION_WAIT:
			CORE__ignition_state = CORE__IGNITION_SM_T_IGNITION_IGNITE;
		break;

		case CORE__IGNITION_SM_T_IGNITION_IGNITE:
			chargemonitor_ignite();
			Serial.print("IG");
			Serial.print(chargemonitor_last_actual_voltage);
			Serial.print(" ");
			Serial.print(chargemonitor_last_after_ignition_voltage);
			CORE__ignition_state = CORE__IGNITION_SM_L_IGNITION_REDO;
		break;

		case CORE__IGNITION_SM_L_IGNITION_REDO:
			if(!chargemonitor_last_ignition_succeeded() && ignition_fail_counter<CORE_IGNITION_RETRY_MAX_COUNT) {
				/* 
					the ignition spark doesn't work or was not sucsessful to trigger the discharg arc.
					reignite th ignition spark and retry.
				*/
				ignition_fail_counter++;
				Serial.print("_F"); // F = fail
				Serial.println(ignition_fail_counter);
				ignition_retry_delay = millis()+CORE_IGNITION_RETRY_DELAY_MILLIS;
				CORE__ignition_state = CORE__IGNITION_SM_T_IGNITION_REDO_DELAY;
			} else if(ignition_fail_counter==0) {
				Serial.println("_G"); // G = good
				CORE__ignition_state = CORE__IGNITION_SM_T_IGNITION_DONE;
			} else {
				ignition_failure_code = 21;
				CORE__ignition_state = CORE__IGNITION_SM_T_ABORT_DUE_FAILURE;
			}
		break;

		case CORE__IGNITION_SM_T_IGNITION_REDO_DELAY:
			// wait for next retry ignition
			if(millis()>=ignition_retry_delay) {
				CORE__ignition_state = CORE__IGNITION_SM_T_IGNITION_IGNITE;
			}
		break;

		case CORE__IGNITION_SM_T_IGNITION_DONE:
			// disarm ignition state machine
			ignition_armed = 0x5A;
			CORE__ignition_state = CORE__IGNITION_SM_L_OFF;
		break;

		case CORE__IGNITION_SM_T_ABORT_DUE_FAILURE:
			// ignition failure handling
			ignition_armed = 0x5A;
			Serial.print("IGF ");
			Serial.println(ignition_failure_code,HEX);
			CORE__ignition_state = CORE__IGNITION_SM_L_OFF;
		break;

	}
	
}

// end: Core
//..............................................................
//..............................................................



//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: master timekeeper

uint8_t TIMEKEEPER__get_current_timeframe() {

	if(DS3231__get_hour()>=CORE__DATATRANSFER_WINDOW_START_HOUR && DS3231__get_hour()<=CORE__DATATRANSFER_WINDOW_END_HOUR) {
		return TIMEKEEPER__DATA_TRANSFER_WINDOW;
	}
	// if it is time for science execution and power state is good, start science bootup sequence
	
	if((DS3231__get_hour()>=0 && DS3231__get_hour()<=CORE__DATATRANSFER_WINDOW_START_HOUR) || (DS3231__get_hour()>=CORE__DATATRANSFER_WINDOW_END_HOUR && DS3231__get_hour()<=23)) {
		if(DS3231__get_minute()>=0 && DS3231__get_minute()<=3) {
			return TIMEKEEPER__SCIENCE_WINDOW;
		}
	}
}

// end: master timekeeper
//..............................................................
//..............................................................



//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: usb data switch unit

bool USB_DATASWITCH_try_switch_to_transfer_mode() {
	return true;
}

bool USB_DATASWITCH_try_switch_to_science_mode() {
	
}

// end: usb data switch unit
//..............................................................
//..............................................................



//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: ignition spark & arc unit

void IGNITION_request_charging(int cap_target_voltage) {
	if(cap_target_voltage>=CORE_IGNITION_MIN_CHARG_VOLTAGE
		&& cap_target_voltage<=CORE_IGNITION_MAX_CHARG_VOLTAGE) {
		ignition_requested_charging_voltage = cap_target_voltage;
	} else {
		ignition_requested_charging_voltage = -1;
	}
}

// end: ignition spark & arc unit
//..............................................................
//..............................................................


//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: PowerLED

void POWERLED_frontlight(int pwm_duty_circle) {
	analogWrite(CORE__PIN_PWM_POWERLED_FRONT, pwm_duty_circle);
}

void POWERLED_backlight(int pwm_duty_circle) {
	analogWrite(CORE__PIN_PWM_POWERLED_BACK, pwm_duty_circle);
}

// end: PowerLED
//..............................................................
//..............................................................



//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: Serial communication functions

void SERIAL__Parser() {
	uint8_t c;	
	while(Serial.available()) {
		c = Serial.read();
		SERIAL__CMDParser(c);
	}
}


void SERIAL__CMDParser(char c) {
	static char buf[32];
	static uint8_t bufcnt = 0;
	static uint8_t bufcntcmd = 0;
	buf[bufcnt++] = c;
	if(c!=0x0a && c!=0x0d) bufcntcmd++;

	if((c == 0x0a) || (bufcnt >= 30)) { // (c == 0x0d) ||
		switch(buf[0]) {
			case 'w': // write
				SERIAL__ParserWrite(&buf[1],bufcntcmd);
			break;
			case 'r': // read
				SERIAL__ParserRead(&buf[1],bufcntcmd);
			break;
			case 'e': // execute
				SERIAL__ParserExecute(&buf[1],bufcntcmd);
			break;
		}

		bufcnt = 0;
		bufcntcmd = 0;
	}
}


void SERIAL__ParserRead(char * buf,uint8_t cnt) {
	cnt-=4;
	switch (buf[0]) {
		case 'd':
			if(buf[1]=='t') {  // read date time from RTC. example rdt returns 2017_9_16_20_14_00
				Serial.println(DS3231__get_Time());
			}
		break;

		case 'h': // requesting current health data
			Serial.println("h");
		break;

	}
}

void SERIAL__ParserWrite(char * buf,uint8_t cnt) {
	long tmpLong = 0L;
	cnt-=4;
	switch (buf[0]) {
		case 'f': // set front LED PWM
			{
				buf[0] = ' ';
				tmpLong = atoi((const char*)&buf[1]);
				Serial.println(tmpLong);
				POWERLED_frontlight((int)tmpLong);
				// analogWrite(POWERLED_FRONT, 100);
			}
		break;

		case 'b': // set back LED PWM
			{
				buf[0] = ' ';
				tmpLong = atoi((const char*)&buf[1]);
				Serial.println(tmpLong);
				POWERLED_backlight((int)tmpLong);
				// analogWrite(POWERLED_BACK, 100);
			}
		break;

		case 'd':
			if(buf[1]=='t') { // write date time to RTC. example wdt2017_9_16_20_14_00
				String str((const char*)&buf[2]);
				DS3231__set_Time(str);
			}
		break;

		case 'c':
			if(buf[1]=='t') { // set arc capcitor charg level
				buf[0] = ' ';
				tmpLong = atoi((const char*)&buf[1]);
				IGNITION_request_charging((int)tmpLong);
				Serial.print("IG CHARG REQUEST ACK ");
				Serial.println(tmpLong);
			}
		break;

		case 's':
			if(buf[1]=='e') { // set arc capcitor charg level
				String str((const char*)&buf[2]);
				DRV2605__effect_sequence = str;
				DRV2605__parse_waveform_string_sequence(DRV2605__effect_sequence); // example config "64_64_64_64_64_64_64"
			}
		break; 
	}
}




void SERIAL__ParserExecute(char * buf,uint8_t cnt) {
	cnt-=4;
	switch (buf[0]) {
		case 's':
			if(buf[1]=='e') {
				Serial.println("shake enabled");
				DRV2605__shaker_run = 1;
			}

			if(buf[1]=='d') {
				Serial.println("shake disabled");
				DRV2605__shaker_run = 2;
			}
		break;

		case 'r':
			if(buf[1]=='r') {
				Serial.println("RPiRdy");
			}
			if(buf[1]=='s') {
				Serial.println("RPiShutdownRequested");
			}
		break;

	}
}


char SERIAL__readSingleHex(char c) {
	char cret = 0x00;
	if(c>='A' && c<='F') {
		cret = c-'A'+10;
	}
	if(c>='a' && c<='f') {
		cret = c-'a'+10;
	}
	if(c>='0' && c<='9') {
		cret = c-'0';
	}
	return cret;
}

// end: Serial communication functions
//..............................................................
//..............................................................





//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: shaker driver DRV2605


void DRV2605__init_if_required() {
	if(DRV2605__shaker.readRegister8(DRV2605_REG_WAVESEQ1)==0) {
		DRV2605__init();
		delay(1);
	}
}


void DRV2605__init() {
	// Configuration for DRV2605_Haptic I2C trigger Modus
	DRV2605__shaker.begin();
	DRV2605__shaker.setMode(DRV2605_MODE_INTTRIG); // default, internal trigger when sending GO command
	DRV2605__shaker.selectLibrary(1); // effect library mode
	DRV2605__shaker.useERM(); // actuactor type set to rotary
	DRV2605__parse_waveform_string_sequence(DRV2605__effect_sequence); // default value
}


/*
 * Sets the config new.
 */
void DRV2605__set_effect_waveform(uint8_t form_1, uint8_t form_2, uint8_t form_3, uint8_t form_4, uint8_t form_5, uint8_t form_6, uint8_t form_7 ) {
	DRV2605__shaker.setWaveform(0, form_1);  
	DRV2605__shaker.setWaveform(1, form_2);  
	DRV2605__shaker.setWaveform(2, form_3);  
	DRV2605__shaker.setWaveform(3, form_4);  
	DRV2605__shaker.setWaveform(4, form_5);  
	DRV2605__shaker.setWaveform(5, form_6);  
	DRV2605__shaker.setWaveform(6, form_7);  // end 
}


/*
 * Runs the DRV2605__shaker.
 */
bool DRV2605__shaker_loop() {
	if (DRV2605__shaker_run==1) { 
		DRV2605__shaker.go();
	} 
	if (DRV2605__shaker_run==2) { 
		DRV2605__shaker.stop();
		DRV2605__shaker_run = 0;
	}
}


/*
 * Set a waveform configuration of exactly 7 values of 0-116.
 * Example "64_64_64_64_14_14_0". 0 means stop.
 * The waveform is looped by run_Drv().
 */
bool DRV2605__parse_waveform_string_sequence (String my_Config) {

	int drv_Config[] = {0,0,0,0,0,0,0};

	//Split String and get Int values
	int j = 0;
	int pos = 0;
	for (int i = 0; i < my_Config.length(); i++) {
		if ((char)my_Config[i] == '_'&& j<7) {
			drv_Config[j] = my_Config.substring(pos, i).toInt();
			j++;
			pos=i+1;
			}
		//Last substring
		if (i==my_Config.length()-1 && j<7) {
			drv_Config[j] = my_Config.substring(pos, my_Config.length()).toInt();
		}
	}

	//Check if the values are ok
	bool my_Config_OK = true;
	if (j!=6) my_Config_OK = false;
	for (int i=0; i<7;i++){
		if (drv_Config [i] <0  || drv_Config [i] > 116) my_Config_OK = false;
	}

	//Set Config
	if (my_Config_OK) {
		DRV2605__set_effect_waveform(drv_Config [0],drv_Config [1],drv_Config [2],drv_Config [3],drv_Config [4],drv_Config [5],drv_Config [6]);
		return 1;
	} else return 0;
}



// end: shaker driver DRV2605
//..............................................................
//..............................................................






//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: RTC code DS3231




/*Returns the RTC Time as String YYYY_M_D_H_M_S*/
String DS3231__get_Time() {
	DateTime now = rtc.now();
	String current_Time = String( now.year());

	current_Time += "_" + String( now.month());
	current_Time += "_" + String( now.day());
	current_Time += "_" + String( now.hour());
	current_Time += "_" + String( now.minute());
	current_Time += "_" + String( now.second());
	return current_Time;
}

long DS3231__get_unixtime() {
	DateTime now = rtc.now();
	return now.unixtime();
}

long DS3231__get_hour() {
	DateTime now = rtc.now();
	return now.hour();
}

long DS3231__get_minute() {
	DateTime now = rtc.now();
	return now.minute();
}

/*Sets the rtc to a new time. Sring format is: 
 * YYYY_M_D_H_M_S (no leading 0s)
 */
void DS3231__set_Time(String new_Time) {

	int rtc_Time[] = {0,0,0,0,0,0};

	int j = 0;
	int pos = 0;
	for (int i = 0; i < new_Time.length(); i++) {
		if ((char)new_Time[i] == '_'&& j<6) {
			rtc_Time[j] = new_Time.substring(pos, i).toInt();
			j++;
			pos=i+1;
		}
		if (i==new_Time.length()-1&& j<6) {
			rtc_Time[j] =  new_Time.substring(pos, new_Time.length()).toInt();
		}
	}

	if (j<6) rtc.adjust(DateTime((uint16_t)rtc_Time[0],rtc_Time[1],rtc_Time[2],rtc_Time[3],rtc_Time[4],rtc_Time[5]));
}

// end: RTC code DS3231
//..............................................................
//..............................................................


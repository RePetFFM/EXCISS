
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

uint8_t CORE__operation_mode = CORE__OPERATION_MODE_RECOVERY;


// global states
uint32_t CORE__init_done = CORE__INIT_EXECUTE; // 0xA5A5 = init run state, 0x5A5A = init done


// init state machine
uint32_t CORE__init_state = CORE__INIT_STATE_I2C_BABYSITTER; // the state char are mirrord 4bit values. For example the 4bit value 0101 
char init_i2c_failure_codes[3];// failure code variales
char init_failure_code = 0;

// main state machine
uint32_t CORE__main_state = CORE__MAIN_SM_L_RECOVERMODE_DATATRANSFER_MODE; // the state char are mirrord 4bit values. For example the 4bit value 0101 
uint8_t CORE__statemachine_retry = 0;

// powermanagment state machine
uint32_t CORE__powermanagment_state = CORE__POWER_SM_T_INIT; // the state char are mirrord 4bit values. For example the 4bit value 0101 

// highvoltage state machine
uint32_t CORE__ignition_state = CORE__IGNITION_SM_L_OFF;

// char ignition_armed = 0x00; // 0xA5 = armed, 0x5A = disarmed
int ignition_charging_capacity_target_voltage = 0; // must be configured via serial from raspberry pi.
char ignition_failure_code = 0;  // 00 - FF
uint8_t ignition_status = 0;

unsigned long science_computer_keepalive_timer;
unsigned long science_computer_wakeup_timer;

uint8_t ingition_requested = 0;


void setup() {
	// wdt_enable(WDTO_8S); // set watchdog interval to 8 seconds

	Serial.begin( UART_BAUTRATE );
	Serial.println("serial rdy");

	Wire.begin(); // Initialize I2C master

	
	CORE__init_pins();
	

	wdt_reset(); // reset watchdog timer
}


void loop() {
	if(CORE__init_done == CORE__INIT_EXECUTE) {
		// Serial.println(CORE__init_state,HEX);
		CORE__statemachine_init();
	} 

	if(CORE__init_done == CORE__INIT_DONE) {
		// Serial.println(CORE__main_state);
		powermanager_poll_powercycle_command(); // VBUS power event handling


		SERIAL__Parser();


		CORE__statemachine_powermanagment(); // power
		// CORE__task_powermanagment(); // power


		CORE__statemachine_main();


		CORE__statemachine_ignition();

	}

	wdt_reset(); // reset watchdog timer

	
}

//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: Core

void CORE__init_pins() {
	// set higher pwm frequency for the leds to prevent flicker in video recording
	TCCR1B = TCCR1B & 0b11111000 | 0x01; // 0x01 = 31250Hz/1, 0x02 = 31250Hz/8

	// TODO: remove following pullup pin mode if serial works fine!
	// pinMode(0, INPUT_PULLUP);

	// left side
	pinMode(CORE__PIN_DOUT_MOSFET_5V, OUTPUT);
	pinMode(CORE__PIN_DOUT_MOSFET_8V, OUTPUT);
	pinMode(CORE__PIN_DOUT_FORCE_RASPI_SHUTDOWN, OUTPUT);
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
	digitalWrite(CORE__PIN_DOUT_FORCE_RASPI_SHUTDOWN, LOW);
	digitalWrite(CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER, LOW);
	digitalWrite(CORE__PIN_DOUT_SCIENCE_ARC_CHARG, LOW);

	powermanager_vsys_on();
	POWERLED_frontlight(0);
	POWERLED_backlight(0);

	powermanager_8V_off();
	powermanager_5V_off();
	USB_DATASWITCH_switch_to_transfer_mode();
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


void CORE__statemachine_init() {

	static uint32_t laststate = 0;
	if(laststate!=CORE__init_state) {
		laststate = CORE__init_state;
		Serial.print("sm");
		Serial.println(CORE__init_state);
	}

	switch (CORE__init_state) {
		case CORE__INIT_STATE_I2C_BABYSITTER:
			uint8_t powermanager_i2c_init;
			Serial.println("bs req1");
			powermanager_i2c_init = powermanager_begin();
			Serial.println("bs req2");
			if (!powermanager_i2c_init && (CORE__statemachine_retry<10)) {
				// retry if init was not successful
				CORE__statemachine_retry++;
				init_i2c_failure_codes[0] = 0xA0;
				init_i2c_failure_codes[0] &= CORE__statemachine_retry;
				delay(10);
			} else if(!powermanager_i2c_init && CORE__statemachine_retry>=10) {
				init_i2c_failure_codes[0] = 0xF1; // major failor babysitter doesen't responde after 10 try's
				Serial.println("bs failure");
				CORE__init_state = CORE__INIT_STATE_I2C_RTC;
				CORE__statemachine_retry = 0;
			} else {
				CORE__init_state = CORE__INIT_STATE_I2C_RTC;
				CORE__statemachine_retry = 0;	
			}
			
		break;

		case CORE__INIT_STATE_I2C_RTC:
			uint8_t rtc_i2c_init;
			rtc_i2c_init = rtc.begin();
			if (!rtc_i2c_init && (CORE__statemachine_retry<10)) {
				// retry if init was not successful
				CORE__statemachine_retry++;
				init_i2c_failure_codes[1] = 0xA0;
				init_i2c_failure_codes[1] &= CORE__statemachine_retry;
				delay(10);
			} else if(!rtc_i2c_init && CORE__statemachine_retry>=10) {
				init_i2c_failure_codes[1] = 0xF1; // major failor babysitter doesen't responde after 10 try's
				CORE__init_state = CORE__INIT_STATE_END;
				CORE__statemachine_retry = 0;
			} else {
				CORE__statemachine_retry = 0;
				CORE__init_state = CORE__INIT_STATE_END;
			}
		break;

		case CORE__INIT_STATE_END:
			CORE__init_done 	= CORE__INIT_DONE;
		break;
	}
}

void CORE__statemachine_main() {
	static unsigned long main_statemachine_delay = 0;
	static uint32_t CORE__main_sm_delay;
	static uint32_t CORE__main_sm_delay_next_state;

	
	static uint32_t laststate = 0;
	if(laststate!=CORE__main_state) {
		laststate = CORE__main_state;
		Serial.print("sm");
		Serial.println(CORE__main_state);
	}

	if(CORE__main_sm_delay>0 && millis()>CORE__main_sm_delay) {
		CORE__main_sm_delay = 0;
		CORE__main_state = CORE__main_sm_delay_next_state;
		CORE__main_sm_delay_next_state = 0;
	}


	switch (CORE__main_state) {
		case CORE__MAIN_SM_L_RECOVERMODE_DATATRANSFER_MODE:
			// CORE__main_state = CORE__MAIN_SM_L_IDLE;
			CORE__powermanagment_state = CORE__POWER_SM_ENTER_RECOVERY_MODE_DATATRANSFER;
			CORE__main_state = CORE__MAIN_SM_DELAY;
			CORE__main_sm_delay_next_state = CORE__MAIN_SM_L_IDLE;
			CORE__main_sm_delay = millis()+CORE__MAIN_SM_DELAY_EXIT_RECOVERYMODE_MILLIS;
			// the recovery mode is handled by powermanagment state machine
		break;

		case CORE__MAIN_SM_L_IDLE:
			// Serial.println(TIMEKEEPER__get_current_timeframe());
			switch(TIMEKEEPER__get_current_timeframe()) {
				case TIMEKEEPER__DATA_TRANSFER_WINDOW:
					USB_DATASWITCH_switch_to_transfer_mode();
				break;

				case TIMEKEEPER__SCIENCE_WINDOW:
					if(powermanager_has_power_for_experiment()) {
						// TODO!!! Check for minimum execution time until transfer time frame 
						if(millis()>science_computer_wakeup_timer) {
							CORE__main_state = CORE__MAIN_SM_T_SCIENCE_GO;	
						}
					}
				break;
			}
		break;

		
		case CORE__MAIN_SM_T_SCIENCE_GO:
			// switch USB to raspberry pi
			// flush serial buffer and enable serial parser
			USB_DATASWITCH_switch_to_science_mode();
			powermanager_8V_on();
			main_statemachine_delay = millis()+100; // wait for 100 millis befor turn on raspberry pi
			CORE__main_state = CORE__MAIN_SM_T_SCIENCE_GO_DELAY;
		break;

		case CORE__MAIN_SM_T_SCIENCE_GO_DELAY:
			if(millis()>=main_statemachine_delay) {
				CORE__main_state = CORE__MAIN_SM_T_SCIENCE_GO_RASPI_POWERUP;
			}
		break;

		case CORE__MAIN_SM_T_SCIENCE_GO_RASPI_POWERUP:
			// powering up 5V switched power rail. The raspberry pi will begin with booting
			// ignition_armed = 0xA5;
			science_computer_keepalive_timer = millis()+SCIENCE__DEFAULT_KEEPALIVE_TIME_MILLIS;

			powermanager_5V_on();

			CORE__ignition_state = CORE__IGNITION_SM_L_IDLE;

			CORE__main_state = CORE__MAIN_SM_L_SCIENCE_RASPI_KEEPALIVE;
		break;

		case CORE__MAIN_SM_L_SCIENCE_RASPI_KEEPALIVE:
			if(millis()>science_computer_keepalive_timer) {
				OPERATIONS__send_raspberry_shutdown_signal();

				CORE__ignition_state = CORE__IGNITION_SM_L_OFF;

				main_statemachine_delay = millis() + SCIENCE__DEFAULT_WAIT_UNTIL_POWERDOWN;

				CORE__main_state = CORE__MAIN_SM_T_SCIENCE_POWERDOWN_DELAY;
			}
		break;

		case CORE__MAIN_SM_T_SCIENCE_POWERDOWN_DELAY:
			if(millis()>main_statemachine_delay) {
				CORE__operation_mode = CORE__OPERATION_MODE_SCIENCE; 
				CORE__powermanagment_state = CORE__POWER_SM_L_IDLE_MODE;
				powermanager_8V_off();
				powermanager_5V_off();
				USB_DATASWITCH_switch_to_transfer_mode();
				CORE__main_state = CORE__MAIN_SM_L_IDLE;
				if(millis()>science_computer_wakeup_timer) {
					// if wakeup timer not set, set default timer. 
					science_computer_wakeup_timer = millis() + SCIENCE__DEFAULT_WAKEUP_TIMER_MILLIS;
				}
			}
		break;
	}
}

unsigned long timestamp_recoverymodus_requested;

void CORE__statemachine_powermanagment() {
	static unsigned long delayTimer;
	static uint32_t CORE__powermanagment_sm_delay;
	static uint32_t CORE__powermanagment_sm_delay_next_state;

	static uint32_t laststate = 0;
	if(laststate!=CORE__powermanagment_state) {
		laststate = CORE__powermanagment_state;
		Serial.print("sm");
		Serial.println(CORE__powermanagment_state);
	}

	if(CORE__powermanagment_sm_delay>0 && millis()>CORE__powermanagment_sm_delay) {
		CORE__powermanagment_sm_delay = 0;
		CORE__powermanagment_state = CORE__powermanagment_sm_delay_next_state;
		CORE__powermanagment_sm_delay_next_state = 0;
	}
	
	switch (CORE__powermanagment_state) {
		case CORE__POWER_SM_T_INIT:
			CORE__powermanagment_state = CORE__POWER_SM_L_IDLE_MODE;
		break;

		case CORE__POWER_SM_L_IDLE_MODE:
			if(powermanager_has_command_reload_config()) {
				Serial.println("reload config");
				timestamp_recoverymodus_requested = millis();
				OPERATIONS__send_raspberry_shutdown_signal();
				
				// set delay. Waiting for gracefull SCU shutdown state.
				CORE__powermanagment_state = CORE__POWER_SM_DELAY;
				CORE__powermanagment_sm_delay_next_state = CORE__POWER_SM_ENTER_RECOVERY_MODE_DATATRANSFER;	
				CORE__powermanagment_sm_delay = millis() + CORE__POWER_SM_DELAY_ENTER_RECOVERYMODE_MILLIS;
			}

			if(powermanager_shutdown_requested()==0x5A) {
				OPERATIONS__send_raspberry_shutdown_signal();
			}
		break;
		
		case CORE__POWER_SM_ENTER_RECOVERY_MODE_DATATRANSFER:
			CORE__operation_mode = CORE__OPERATION_MODE_RECOVERY;
			CORE__powermanagment_state = CORE__POWER_SM_DELAY;
			CORE__powermanagment_sm_delay_next_state = CORE__POWER_SM_L_RECOVERY_MODE_START_SCU;	
			CORE__powermanagment_sm_delay = millis() + CORE__POWER_SM_DELAY_ENTER_SCU_POWERON_MILLIS;
		break;

		case CORE__POWER_SM_L_RECOVERY_MODE_START_SCU:
			science_computer_keepalive_timer = millis()+CORE__POWER_MIN_KEEPALIVE_TIME_MILLIS;
			powermanager_5V_on();
			CORE__ignition_state = CORE__IGNITION_SM_L_OFF;
			CORE__main_state = CORE__MAIN_SM_L_SCIENCE_RASPI_KEEPALIVE;
			USB_DATASWITCH_switch_to_science_mode();

			CORE__powermanagment_state = CORE__POWER_SM_DELAY;
			CORE__powermanagment_sm_delay_next_state = CORE__POWER_SM_L_IDLE_MODE;	
			CORE__powermanagment_sm_delay = millis() + CORE__MAIN_SM_DELAY_EXIT_RECOVERYMODE_MILLIS;
		break;

	}
	
}

uint8_t ignition_fail_counter = 0;

int ignition_requested_charging_voltage = 0;

void CORE__statemachine_ignition() {
	static unsigned long ignition_max_charging_runtime; 
	static unsigned long ignition_retry_delay;

	static uint32_t laststate = 0;
	if(laststate!=CORE__ignition_state) {
		laststate = CORE__ignition_state;
		Serial.print("sm");
		Serial.println(CORE__ignition_state);
	}

	
	switch (CORE__ignition_state) {

		case CORE__IGNITION_SM_L_OFF:
			ignition_requested_charging_voltage = 0;
			ignition_charging_capacity_target_voltage = 0;
			ignition_status = 0;
			ingition_requested = false;
		break;

		case CORE__IGNITION_SM_L_IDLE:
			// wait state for charg reques from raspberry pi
			if(ignition_requested_charging_voltage>=CORE_IGNITION_MIN_CHARG_VOLTAGE) {
				ignition_charging_capacity_target_voltage = ignition_requested_charging_voltage;
				if(ignition_charging_capacity_target_voltage>0) {
					chargemonitor_start_charging(ignition_charging_capacity_target_voltage);	
					ignition_max_charging_runtime = millis() + CORE_IGNITION_MAX_CHARGTIME_MILLIS; // 300 second max charging time
					ignition_status = 1;
					CORE__ignition_state = CORE__IGNITION_SM_L_CHARGE;
				} else {
					// if cap charg voltage target is 0 (init default value) abort ignition
					// send failure code to raspberry pi
					ignition_status = 255;
					chargemonitor_abort();
					ignition_failure_code = 11;
					CORE__ignition_state = CORE__IGNITION_SM_T_ABORT_DUE_FAILURE;
				}
			} else if(ignition_requested_charging_voltage==-1) {
				ignition_charging_capacity_target_voltage = 0;
				ignition_failure_code = 1;
				CORE__ignition_state = CORE__IGNITION_SM_T_ABORT_DUE_FAILURE;
			}
		
		break;

		
		case CORE__IGNITION_SM_L_CHARGE:
			// charging arc capacitor until requested voltage value
			ignition_status = 1;
			// Serial.println(chargemonitor_get_charge());
			if(chargemonitor_has_enough_charge()) {
				chargemonitor_stop_charging();
				CORE__ignition_state = CORE__IGNITION_SM_T_IGNITION_READY;
				ignition_fail_counter = 0;
				ignition_status = 2;
			}

			if(millis()>=ignition_max_charging_runtime) {
				// max chargingtime was exceeded abort charging
				chargemonitor_abort();
				ignition_status = 255;
				ignition_failure_code = 12;
				CORE__ignition_state = CORE__IGNITION_SM_T_ABORT_DUE_FAILURE;
			}
		break;

		case CORE__IGNITION_SM_T_IGNITION_READY:
			if(ingition_requested) {
				CORE__ignition_state = CORE__IGNITION_SM_T_IGNITION_IGNITE;	
			}
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
				Serial.print("IG_F"); // F = fail
				Serial.println(ignition_fail_counter);
				ignition_retry_delay = millis()+CORE_IGNITION_RETRY_DELAY_MILLIS;
				CORE__ignition_state = CORE__IGNITION_SM_T_IGNITION_REDO_DELAY;
				ingition_requested = false;
			} else if(ignition_fail_counter==0) {
				Serial.println("IG_GOOD"); // G = good
				ignition_status = 0;
				CORE__ignition_state = CORE__IGNITION_SM_L_OFF;
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

		case CORE__IGNITION_SM_T_ABORT_DUE_FAILURE:
			// ignition failure handling
			// ignition_armed = 0;
			ingition_requested = false;
			ignition_status = 0;
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
// begin: operations routines

uint8_t TIMEKEEPER__get_current_timeframe() {
	if(DS3231__get_hour()>=CORE__DATATRANSFER_WINDOW_START_HOUR && DS3231__get_hour()<=CORE__DATATRANSFER_WINDOW_END_HOUR) {
		return TIMEKEEPER__DATA_TRANSFER_WINDOW;
	}
	
	return TIMEKEEPER__SCIENCE_WINDOW;
}


uint32_t TIMEKEEPER__get_deltaT_to_next_datatransfer_seconds() {
	DateTime cdt = rtc.now();
	uint32_t current_date_time_ut = DS3231__get_unixtime();

	if(DS3231__get_hour()>CORE__DATATRANSFER_WINDOW_END_HOUR) {
		return DateTime(cdt.year(),cdt.month(),cdt.day()+1,CORE__DATATRANSFER_WINDOW_START_HOUR,0,0).unixtime()-current_date_time_ut;
	}

	if(DS3231__get_hour()<CORE__DATATRANSFER_WINDOW_START_HOUR) {
		return DateTime(cdt.year(),cdt.month(),cdt.day(),CORE__DATATRANSFER_WINDOW_START_HOUR,0,0).unixtime()-current_date_time_ut;
	}

	return 0;
}



void OPERATIONS__send_raspberry_shutdown_signal() {
	digitalWrite(CORE__PIN_DOUT_FORCE_RASPI_SHUTDOWN, HIGH);
	delay(1);	
	digitalWrite(CORE__PIN_DOUT_FORCE_RASPI_SHUTDOWN, LOW);
}

// end: operations routines
//..............................................................
//..............................................................



//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: usb data switch unit

bool USB_DATASWITCH_switch_to_transfer_mode() {
	digitalWrite(CORE__PIN_DOUT_USB_SWITCH_SWITCH, USBSWITCH__SWITCH_TRANSFER);
	return true;
}

bool USB_DATASWITCH_switch_to_science_mode() {
	digitalWrite(CORE__PIN_DOUT_USB_SWITCH_SWITCH, USBSWITCH__SWITCH_SCIENCE);
	return true;
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

		CORE__ignition_state = CORE__IGNITION_SM_L_IDLE;
		Serial.println(ignition_requested_charging_voltage);
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
// begin: RTC code DS3231

/*Returns the RTC Time as String YYYY_M_D_H_M_S*/
String DS3231__get_Time() {
	DateTime now = rtc.now();
	String current_Time = String( now.year());

	// YYYY-MM-DDThh:mm:ssTZD

	current_Time += "-" + String( now.month());
	current_Time += "-" + String( now.day());
	current_Time += "T" + String( now.hour());
	current_Time += ":" + String( now.minute());
	current_Time += ":" + String( now.second());
	return current_Time;
}

uint32_t DS3231__get_unixtime() {
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
void DS3231__set_Time(String new_Time, bool timeRecoverMode) {

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

	if(timeRecoverMode) {
		if (j<6) {
			// uint32_t recover_start_datetime_ut = DateTime((uint16_t)rtc_Time[0],rtc_Time[1],rtc_Time[2],rtc_Time[3],rtc_Time[4],rtc_Time[5]).uint32_t();
			// recover_start_datetime_ut + recover_mode_last_entered_ut;
			// rtc.adjust();
		}
	} else {
		if (j<6) rtc.adjust(DateTime((uint16_t)rtc_Time[0],rtc_Time[1],rtc_Time[2],rtc_Time[3],rtc_Time[4],rtc_Time[5]));
	}

	
}

// end: RTC code DS3231
//..............................................................
//..............................................................




//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: Serial communication functions


void SERIAL__Parser() {
	static uint8_t cmd[32];         // command buffer
	static uint8_t cmdcount = 0;    // position in the buffer of the received byte
	uint8_t c;                      // received byte
	while(Serial.available()) {
		c = Serial.read();
		if(c > ' ') cmd[cmdcount++] = c;
		if((c == 8) && (cmdcount > 0)) cmdcount--;                // deals with backspaces, if a person on the other side types 
		if((c == 0x0d) || (c == 0x0a) || (cmdcount > 32)) {       // end of command, gets interpreted now
			cmd[cmdcount] = 0;    // clear the last byte in cmd buffer
			if(cmdcount > 2) {    // prevent empty cmd buffer parsing
				switch(cmd[0]) {
					case 'w': // write
					SERIAL__ParserWrite(&cmd[1],cmdcount);
					break;
					case 'r': // read
					SERIAL__ParserRead(&cmd[1],cmdcount);
					break;
					case 'e': // execute
					SERIAL__ParserExecute(&cmd[1],cmdcount);
					break;           
				}    
			}
			cmdcount = 0;
		} 
	}
}


void SERIAL__ParserRead(char * buf,uint8_t cnt) {
	cnt-=4;
	switch (buf[0]) {
		case 'd':
			if(buf[1]=='t') {
				// set date time: 
				// command: wdtYYYY_MM_DD_HH_MM_SS
				// example: wdt2017_9_17_12_30_00
				// note: don't user leading zeros
				Serial.println(DS3231__get_Time());
			}
		break;

		case 'o':
			if(buf[1]=='m') {
				// get current operation mode
				// command: rom
				// returns: returns current operation mode RECOVERY / SCIENCE
				switch(CORE__operation_mode) {
					case CORE__OPERATION_MODE_RECOVERY: 
						Serial.println("RECOVERY");
					break;
					case CORE__OPERATION_MODE_SCIENCE: 
						Serial.println("SCIENCE");
					break;
				}
			}
		break;

		case 'h':
			if(buf[1]=='r') {
				// get system health data from arduino:
				// command: rh
				// parameter: none
				// returns: system health data from arduino
				Serial.println("MCU health log");
				Serial.print("bat v: ");
				Serial.println(powermanager_get_voltage());
				Serial.print("bat a: ");
				Serial.println(powermanager_get_capacity());
				Serial.print("bat charg: ");
				Serial.println(powermanager_get_charge_state());
				Serial.print("frame power: ");
				powermanager_get_usb_power_status() ? Serial.println("FRAME POWER ONLINE") : Serial.println("FRAME POWER OFFLINE");					
			}		
		break;

		case 'b':
			if(buf[1]=='v') {
				// get batterie volate:
				// command: rbv
				// returns: voltage in integer value.
				Serial.println(powermanager_get_voltage());
			}

			if(buf[1]=='c') {
				// get batterie capacity:
				// command: rbc
				// returns: milliamper in integer value. 
				Serial.println(powermanager_get_capacity());
			}
		break;

		case 'u':
			if(buf[1]=='s') {
				// get usb power status:
				// command: rus
				// returns: FRAME POWER ONLINE / FRAME POWER OFFLINE
				powermanager_get_usb_power_status() ? Serial.println("FRAME POWER ONLINE") : Serial.println("FRAME POWER OFFLINE");				
			}
		break;


		case 't':
			if(buf[1]=='l') {
				// get time left until next datatransfer start:
				// command: rtd
				// returns: returns time left until next datatransfer start in seconds
				// time_t
				Serial.println(TIMEKEEPER__get_deltaT_to_next_datatransfer_seconds());
			}
		break;


		case 'i':
			if(buf[1]=='v') {
				// get current capacitor voltage
				// command: riv
				// returns: returns capacitor voltage
				Serial.println(chargemonitor_get_charge());
			}
		break;


		


		case 'c':
			if(buf[1]=='r') {
				// TODO: charging error reporting
				switch (ignition_status) {
				    case 0:
						Serial.println("standby");
						break;
				    case 1:
						Serial.println("charging");
						break;
				    case 2:
						Serial.println("charged");
						break;
				    case 255:
				    	Serial.println("failed");
				    	break;
				}
			}
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
				DS3231__set_Time(str,false);
			}
			if(buf[1]=='r') { // write date time to RTC. example wdt2017_9_16_20_14_00
				String str((const char*)&buf[2]);
				DS3231__set_Time(str,true);
			}
		break;

	}
}


void SERIAL__ParserExecute(char * buf,uint8_t cnt) {
	long tmpLong;
	cnt-=4;
	switch (buf[0]) {
		
		case 'r': // serial command "erk300" set the keepalive to 5min
			if(buf[1]=='k') {
				buf[0] = ' ';
				buf[1] = ' ';
				tmpLong = atoi((const char*)&buf[2]);
				science_computer_keepalive_timer = millis()+(1000*tmpLong);
				Serial.println(tmpLong);
				Serial.println("RPikeepalive");
			}
			if(buf[1]=='w') {
				buf[0] = ' ';
				buf[1] = ' ';
				tmpLong = atoi((const char*)&buf[2]);
				science_computer_wakeup_timer = millis()+(1000*tmpLong);
				Serial.println(tmpLong);
				Serial.println("Wakeuptimer set");
			}
		break;

		case 'i':
			if(buf[1]=='c') { // wct set arc capcitor charg level
				ignition_status = 1;
				buf[0] = ' ';
				buf[1] = ' ';
				tmpLong = atoi((const char*)&buf[2]);
				Serial.print("IG CHARG REQUEST ACK ");
				Serial.println(tmpLong);
				IGNITION_request_charging(tmpLong);
			}

			if(buf[1]=='a') { 
				Serial.println("ABORT");
			}

			if(buf[1]=='i') {
				IGNITION_request_charging(ignition_requested_charging_voltage);
				ingition_requested = true;
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





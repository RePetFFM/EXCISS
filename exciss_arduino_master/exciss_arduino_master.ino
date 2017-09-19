#include <Wire.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include "RTClib.h"
#include "exciss.h"
#include "Adafruit_DRV2605.h"
#include "powermanager.h"


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

uint32_t CORE__init_done = CORE__INIT_EXECUTE; // 0xA5A5 = init run state, 0x5A5A = init done

uint32_t CORE__init_state = CORE__INIT_STATE_SET_PWM_FREQ; // the state char are mirrord 4bit values. For example the 4bit value 0101 

uint32_t CORE__main_state = 0x00000000; // the state char are mirrord 4bit values. For example the 4bit value 0101 

uint32_t CORE__powermanagment_state = 0x00000000; // the state char are mirrord 4bit values. For example the 4bit value 0101 

void loop() {
	if(CORE__init_done == CORE__INIT_EXECUTE) {
		CORE__statemachine_init();
	} 

	if(CORE__init_done == CORE__INIT_DONE) {
		SERIAL__Parser();

		CORE__statemachine_main();

		CORE__statemachine_powermanagment();

		DRV2605__shaker_loop();
	}

	CORE_statemachine_state_index_validator();

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
	pinMode(CORE__PIN_DIN_BABYSITTER_PGOOD, INPUT);
	pinMode(CORE__PIN_DIN_BABYSITTER_GPOUT, INPUT);
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

char CORE__eeprom_raid_read_char(int address) {
	char c1,c2,c3;
	c1 = EEPROM.read(address);
	c2 = EEPROM.read(address+1);
	c3 = EEPROM.read(address+2);
	if(c1==c2 && c2==c3) {

	}
}

void CORE__eeprom_raid_write_char(int address,char value) {
	EEPROM.update(address,value);
	EEPROM.update(address+1,value);
	EEPROM.update(address+2,value);
}

void CORE_statemachine_state_index_validator() {
	// todo validate 

	// CORE__init_state
	// CORE__main_state
	// CORE__statemachine_powermanagment
}

void CORE__statemachine_init() {
	switch (CORE__init_state) {
		case CORE__INIT_STATE_SET_PWM_FREQ:
			// set higher pwm frequency for the leds to prevent flicker in video recording
			TCCR1B = TCCR1B & 0b11111000 | 0x01; // 0x01 = 31250Hz/1, 0x02 = 31250Hz/8
			CORE__init_state 	= CORE__INIT_STATE_SET_I2C_COMPONENT_TO_INIT_STATE;
		break;
		case CORE__INIT_STATE_SET_I2C_COMPONENT_TO_INIT_STATE:
			CORE__init_i2c_components();
			CORE__init_state 	= CORE__INIT_STATE_INIT_SERIAL;
		break;
		case CORE__INIT_STATE_INIT_SERIAL:
			Serial.begin( UART_BAUTRATE );
			CORE__init_state 	= CORE__INIT_STATE_END;
			CORE__init_done 	= CORE__INIT_DONE;
		break;
	}
}

void CORE__statemachine_main() {
	switch (CORE__main_state) {
		case :
		// do something
		break;
		case :
		// do something
		break;
		default:
		// do something
	}
}

void CORE__statemachine_powermanagment() {
	switch (CORE__powermanagment_state) {
		case :
		// do something
		break;
		case :
		// do something
		break;
		default:
		// do something
	}
}

// end: Core
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

		case 'h':
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
			if(buf[1]=='s') {
				Serial.println("raspberry power down requested");
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


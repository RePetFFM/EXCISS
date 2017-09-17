#include <Wire.h>
#include "RTClib.h"
#include "exciss.h"
#include "Adafruit_DRV2605.h"
#include "elapsedMillis.h"


// RTC
RTC_DS3231 rtc;


// Globals for DRV2605_Haptic
Adafruit_DRV2605 DRV2605_shaker;
elapsedMillis time_Elapsed;
unsigned long start_ms;
int duration_ms;

uint8_t DRV2605_shaker_run = 0;


void setup() {
	pinMode(POWERLED_FRONT, OUTPUT);
	pinMode(POWERLED_BACK, OUTPUT);


	POWERLED_frontlight(0);
	POWERLED_backlight(0);

	Serial.begin(9600);

	Serial.println("ready");

	if (! rtc.begin()) {
		Serial.println("ERROR Couldn't find RTC");
	} else {
		Serial.println("rtc ready");
	}

	// Configuration for DRV2605_Haptic I2C trigger Modus
	DRV2605_shaker.begin();
	DRV2605_shaker.setMode(DRV2605_MODE_INTTRIG); // default, internal trigger when sending GO command
	DRV2605_shaker.selectLibrary(1);
	DRV2605_shaker.useERM();


	delay(500);
	Serial.print("Config ok?: ");
	Serial.println( set_Config_Drv("64_64_64_64_64_64_64"));
	delay(500);
	Serial.print("Run it for 5 secs ok?: ");
	Serial.println(set_Run_Drv("5000"));
}


void loop() {
	SERIAL__Parser();

	DRV2605_shaker_loop();
}



void POWERLED_frontlight(int pwm_duty_circle) {
	analogWrite(POWERLED_FRONT, pwm_duty_circle);
}

void POWERLED_backlight(int pwm_duty_circle) {
	analogWrite(POWERLED_BACK, pwm_duty_circle);
}


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
	}
}




void SERIAL__ParserExecute(char * buf,uint8_t cnt) {
	cnt-=4;
	switch (buf[0]) {
		case 's':
			if(buf[1]=='e') {
				Serial.println("shake enabled");
				DRV2605_shaker_run = 1;
			}

			if(buf[1]=='d') {
				Serial.println("shake disabled");
				DRV2605_shaker_run = 2;
			}
		break;

		case 'r':
			Serial.println("raspberry power down requested");
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


/*
 * Sets the config new.
 */
void config_Drv(uint8_t form_1, uint8_t form_2, uint8_t form_3, uint8_t form_4, uint8_t form_5, uint8_t form_6, uint8_t form_7 ) {
	DRV2605_shaker.setWaveform(0, form_1);  
	DRV2605_shaker.setWaveform(1, form_2);  
	DRV2605_shaker.setWaveform(2, form_3);  
	DRV2605_shaker.setWaveform(3, form_4);  
	DRV2605_shaker.setWaveform(4, form_5);  
	DRV2605_shaker.setWaveform(5, form_6);  
	DRV2605_shaker.setWaveform(6, form_7);  // end 
}


/*
 * Runs the DRV2605_shaker.
 */
bool DRV2605_shaker_loop() {
	if (DRV2605_shaker_run==1) { 
		DRV2605_shaker.go();
	} 
	if (DRV2605_shaker_run==2) { 
		DRV2605_shaker.stop();
		DRV2605_shaker_run = 0;
	}
}


/*
 * Set a waveform configuration of exactly 7 values of 0-116.
 * Example "64_64_64_64_14_14_0". 0 means stop.
 * The waveform is looped by run_Drv().
 */
bool set_Config_Drv (String my_Config) {

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
		config_Drv(drv_Config [0],drv_Config [1],drv_Config [2],drv_Config [3],drv_Config [4],drv_Config [5],drv_Config [6]);
		return 1;
	} else return 0;
}

/*
 * Starts the DRV2605_shaker. Max 30 seconds. 
 */
bool set_Run_Drv (String ms) {
	start_ms = time_Elapsed; 
	if (ms.toInt()<30000) {duration_ms = ms.toInt(); return 1;}
	else {duration_ms = 0; return 0;}
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


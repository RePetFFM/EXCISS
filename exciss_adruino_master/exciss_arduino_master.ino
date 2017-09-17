#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;


void setup() {
	Serial.begin(9600);

	Serial.println("ready");

	if (! rtc.begin()) {
		Serial.println("ERROR Couldn't find RTC");
	} else {
		Serial.println("rtc ready");
	}

}


void loop() {
	SERIAL__Parser();
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
			}
		break;

		case 'b': // set back LED PWM
			{
				buf[0] = ' ';
				tmpLong = atoi((const char*)&buf[1]);
				Serial.println(tmpLong);
			}
		break;

		case 'd':
			if(buf[1]=='t') { // write date time to RTC. example wdt2017_9_16_20_14_00
				String str((const char*)&buf[2]);
				DS3231__set_Time(str);
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
			}

			if(buf[1]=='d') {
				Serial.println("shake disabled");
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


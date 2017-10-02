#include "RTClib.h"

void setup() {
	Serial.begin(9600);

	int rtc_Time[] = {2017,10,1,21,0,0};

	uint32_t last_powercycle_timestamp_seconds = get_last_powercylce_command_timestamp();

	DateTime recover_date_time_unixtime = DateTime((uint16_t)rtc_Time[0],rtc_Time[1],rtc_Time[2],rtc_Time[3],rtc_Time[4],rtc_Time[5]);

	uint32_t recover_current_date_time = recover_date_time_unixtime.unixtime()+last_powercycle_timestamp_seconds;

	DateTime now = DateTime(recover_current_date_time);

	String current_Time = String( now.year());

	// YYYY-MM-DDThh:mm:ssTZD

	current_Time += "-" + String( now.month());
	current_Time += "-" + String( now.day());
	current_Time += "T" + String( now.hour());
	current_Time += ":" + String( now.minute());
	current_Time += ":" + String( now.second());

	Serial.println(current_Time);
}

uint32_t get_last_powercylce_command_timestamp() {
	uint32_t sec = 50000UL/1000UL;
	return sec;
}

void loop() {
	
}

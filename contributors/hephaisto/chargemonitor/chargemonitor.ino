#include "chargemonitor.h"


void setup()
{
	Serial.begin(57600);
	Serial.print("init...");
	chargemonitor_begin();
	Serial.print("initialised\r\n");
	chargemonitor_start_charging(300); // only one ignition
	Serial.print("voltage set\r\n");
}

void loop()
{
	delay(1000);
	Serial.print("polling...\r\n");
	if(chargemonitor_has_enough_charge())
	{
		Serial.print("enough charge, IGNITING!\r\n");
		chargemonitor_ignite();
		if(chargemonitor_last_ignition_succeeded())
			Serial.print("ignition SUCCEEDED!\r\n");
		else
			Serial.print("ignition FAILED!\r\n");
		Serial.print("ignition parameters:\r\n");

		Serial.print(" target voltage: ");
		Serial.print(chargemonitor_last_target_voltage);
		Serial.print("\r\n");

		Serial.print(" actual voltage: ");
		Serial.print(chargemonitor_last_actual_voltage);
		Serial.print("\r\n");

		Serial.print(" after ignition voltage: ");
		Serial.print(chargemonitor_last_after_ignition_voltage);
		Serial.print("\r\n");
	}
}

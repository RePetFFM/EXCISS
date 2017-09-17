#include "powermanager.h"

bool had_reftime_once = false;

void setup()
{
	had_reftime_once = false;
	Serial.begin(57600);
	Serial.print("init...");
	powermanager_begin();
	Serial.print("initialised\r\n");
}

void loop()
{
	delay(1000);
	Serial.print("polling...\r\n");
	powermanager_poll();
	Serial.print("charge state: ");
	Serial.print(powermanager_get_charge_state());
	Serial.print("\r\n");
	
	if(powermanager_has_command_powerdown())
		Serial.print("received POWERDOWN!\r\n");
	if(powermanager_has_new_reference_time())
	{
		Serial.print("received REFTIME!\r\n");
		had_reftime_once = true;
	}
	if(powermanager_has_command_reload_config())
		Serial.print("received CONFIG RELOAD!\r\n");
	if(powermanager_has_power_for_experiment())
		Serial.print("experiment GO!\r\n");
	else
		Serial.print("experiment hold\r\n");
	if(powermanager_has_power_for_small_things())
		Serial.print("enough energy for configuration\r\n");

	if(had_reftime_once)
	{
		Serial.print("system time: ");
		Serial.print(powermanager_current_time());
		Serial.print("seconds since reference time\r\n");
	}
}

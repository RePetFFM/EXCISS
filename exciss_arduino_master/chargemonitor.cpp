#include <Arduino.h>
#include "exciss.h"

int chargemonitor_target_voltage = 0;

int chargemonitor_last_target_voltage;
int chargemonitor_last_actual_voltage;
int chargemonitor_last_after_ignition_voltage;

int chargemonitor_get_charge()
{
	uint16_t current_cap_voltage_raw_analog = analogRead(CORE__PIN_AIN_SCIENCE_ARC_CHARG_VOLTAGE);
	uint16_t current_cap_voltage = map(current_cap_voltage_raw_analog, 0, 1023, 0, CHARGEMONITOR_VOLTAGE_MAX_CHARGE);
	
	return current_cap_voltage;
}

void chargemonitor_begin()
{
	digitalWrite(CORE__PIN_DOUT_SCIENCE_ARC_CHARG, CHARGEMONITOR_CHARGE_DISABLE);
	digitalWrite(CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER, CHARGEMONITOR_IGNITE_DISABLE);
	pinMode(CORE__PIN_DOUT_SCIENCE_ARC_CHARG, OUTPUT);
	pinMode(CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER, OUTPUT);
	chargemonitor_target_voltage = 0;
}

void chargemonitor_start_charging(int voltage)
{
	chargemonitor_target_voltage = voltage;
	digitalWrite(CORE__PIN_DOUT_SCIENCE_ARC_CHARG, CHARGEMONITOR_CHARGE_ENABLE);
}

void chargemonitor_stop_charging()
{
	digitalWrite(CORE__PIN_DOUT_SCIENCE_ARC_CHARG, CHARGEMONITOR_CHARGE_DISABLE);
}

void chargemonitor_abort()
{
	chargemonitor_stop_charging();
	chargemonitor_target_voltage = 0;
}

bool chargemonitor_has_enough_charge()
{
	return chargemonitor_get_charge() >= chargemonitor_target_voltage;
}


void chargemonitor_ignite()
{
	digitalWrite(CORE__PIN_DOUT_SCIENCE_ARC_CHARG, CHARGEMONITOR_CHARGE_DISABLE);
	chargemonitor_last_target_voltage = chargemonitor_target_voltage;
	chargemonitor_last_actual_voltage = chargemonitor_get_charge();

	digitalWrite(CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER, CHARGEMONITOR_IGNITE_IGNITE);
	delay(CHARGEMONITOR_IGNITE_TOGGLE_DURATION);
	digitalWrite(CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER, CHARGEMONITOR_IGNITE_DISABLE);
	delay(CHARGEMONITOR_IGNITION_WAIT-CHARGEMONITOR_IGNITE_TOGGLE_DURATION);

	chargemonitor_last_after_ignition_voltage = chargemonitor_get_charge();
	chargemonitor_target_voltage = 0;
}

bool chargemonitor_last_ignition_succeeded()
{
	return chargemonitor_last_after_ignition_voltage < CHARGEMONITOR_EMPTY_THRESHOLD;
}

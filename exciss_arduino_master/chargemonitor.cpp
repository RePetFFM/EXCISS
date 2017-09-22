#include <Arduino.h>
#include "exciss.h"

int chargemonitor_target_voltage = 0;

int chargemonitor_last_target_voltage;
int chargemonitor_last_actual_voltage;
int chargemonitor_last_after_ignition_voltage;

int chargemonitor_get_charge()
{
	return analogRead(CHARGEMONITOR_INPUT_PIN)*CHARGEMONITOR_VOLTAGE_SCALE;
}

void chargemonitor_begin()
{
	digitalWrite(CHARGEMONITOR_CHARGE_ENABLE_PIN, CHARGEMONITOR_CHARGE_DISABLE);
	digitalWrite(CHARGEMONITOR_IGNITE_PIN, CHARGEMONITOR_IGNITE_DISABLE);
	pinMode(CHARGEMONITOR_CHARGE_ENABLE_PIN, OUTPUT);
	pinMode(CHARGEMONITOR_IGNITE_PIN, OUTPUT);
	chargemonitor_target_voltage = 0;
}

void chargemonitor_start_charging(int voltage)
{
	chargemonitor_target_voltage = voltage;
	digitalWrite(CHARGEMONITOR_CHARGE_ENABLE_PIN, CHARGEMONITOR_CHARGE_ENABLE);
}

void chargemonitor_stop_charging()
{
	digitalWrite(CHARGEMONITOR_CHARGE_ENABLE_PIN, CHARGEMONITOR_CHARGE_DISABLE);
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
	digitalWrite(CHARGEMONITOR_CHARGE_ENABLE_PIN, CHARGEMONITOR_CHARGE_DISABLE);
	chargemonitor_last_target_voltage = chargemonitor_target_voltage;
	chargemonitor_last_actual_voltage = chargemonitor_get_charge();

	digitalWrite(CHARGEMONITOR_IGNITE_PIN, CHARGEMONITOR_IGNITE_IGNITE);
	delay(CHARGEMONITOR_IGNITE_TOGGLE_DURATION);
	digitalWrite(CHARGEMONITOR_IGNITE_PIN, CHARGEMONITOR_IGNITE_DISABLE);
	delay(CHARGEMONITOR_IGNITION_WAIT-CHARGEMONITOR_IGNITE_TOGGLE_DURATION);

	chargemonitor_last_after_ignition_voltage = chargemonitor_get_charge();
	chargemonitor_target_voltage = 0;
}

bool chargemonitor_last_ignition_succeeded()
{
	return chargemonitor_last_after_ignition_voltage < CHARGEMONITOR_EMPTY_THRESHOLD;
}

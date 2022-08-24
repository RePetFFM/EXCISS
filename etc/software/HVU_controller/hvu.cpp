#include <Arduino.h>
#include "config.h"
#include "portmap.h"

int HVU__target_voltage = 0;

int HVU__last_target_voltage;
int HVU__last_actual_voltage;
int HVU__last_after_ignition_voltage;

int HVU__get_charge()
{
	uint16_t current_cap_voltage_raw_analog = analogRead(AIN_HVU_MEASURE);
	uint16_t current_cap_voltage = map(current_cap_voltage_raw_analog, 0, 1023, 0, HVU_VOLTAGE_MAX_CHARGE);
	
	return current_cap_voltage;
}

void HVU__start_charging(int voltage)
{
	HVU__target_voltage = voltage;
	digitalWrite(DOUT_HVU_600V_ENABLE, HVU_ENABLE);
}

void HVU__stop_charging()
{
	digitalWrite(DOUT_HVU_600V_ENABLE, HVU_DISABLE);
}

void HVU__abort()
{
	HVU__stop_charging();
	HVU__target_voltage = 0;
}

bool HVU__has_enough_charge()
{
	return HVU__get_charge() >= HVU__target_voltage;
}


void HVU__ignite()
{
	
	HVU__last_target_voltage = HVU__target_voltage;
	HVU__last_actual_voltage = HVU__get_charge();

	digitalWrite( DOUT_HVU_600V_ENABLE , HVU_DISABLE );
	digitalWrite( DOUT_HVU_TRIGGER , HVU_TRIGGER_ENABLE );
	delay( HVU_IGNITE_TOGGLE_DURATION );
	digitalWrite( DOUT_HVU_TRIGGER , HVU_TRIGGER_DISABLE );
	delay( HVU_MEASURE_WAIT_AFTER_IGNITION - HVU_IGNITE_TOGGLE_DURATION );

	HVU__last_after_ignition_voltage = HVU__get_charge();
	HVU__target_voltage = 0;
}

bool HVU__last_ignition_succeeded()
{
	return HVU__last_after_ignition_voltage < HVU_DISCHARGE_SUCCESS_THRESHOLD;
}

#include <SparkFunBQ27441.h>
#include "exciss.h"


uint8_t powermanager_status = 0;
uint64_t last_timeref_millis = 0;
uint8_t last_state = 0;
uint64_t last_down_flank = 0;
uint64_t last_cycle_end = 0;
uint8_t cycle_count = 0;

inline uint8_t get_pgood()
{
	return digitalRead(CORE__PIN_DOUT_BABYSITTER_PGOOD) == HIGH;
}

void powermanager_begin()
{
	pinMode(CORE__PIN_DOUT_BABYSITTER_PGOOD, INPUT_PULLUP);
	lipo.begin();
	lipo.setCapacity(POWERMANAGER_CELL_CAPACITY);
}
uint16_t powermanager_get_charge_state()
{
	return lipo.soc();
}
void powermanager_poll()
{
	// check power cycles
	uint8_t pgood = get_pgood();
	if(pgood != last_state)
	{
		if(pgood) // up flank
		{
			uint64_t down_time = millis() - last_down_flank;
			if( (down_time > POWERMANAGER_T_OFF_MIN) && (down_time < POWERMANAGER_T_OFF_MAX) )
			{
				cycle_count++;
				last_cycle_end = millis();
			}
		}
		else // down flank
		{
			last_down_flank = millis();
		}
		last_state = pgood;
	}

	if(cycle_count > 0)
	{
		if(millis() - last_cycle_end > POWERMANAGER_T_WAIT_MAX) // end of command
		{
			if(cycle_count == POWERMANAGER_N_POWERDOWN)
				powermanager_status |= (1<<POWERMANAGER_STATUSBIT_HAS_POWERDOWN);

			if(cycle_count == POWERMANAGER_N_RELOADCONFIG)
				powermanager_status |= (1<<POWERMANAGER_STATUSBIT_HAS_RELOADCONFIG);

			if(cycle_count == POWERMANAGER_N_TIMEREF)
			{
				powermanager_status |= (1<<POWERMANAGER_STATUSBIT_HAS_TIMEREF);
				last_timeref_millis = millis();
			}
			cycle_count = 0;
		}
	}
}

// power-cycle command requests
uint8_t powermanager_has_command_powerdown()
{
	uint8_t result = powermanager_status & (1<<POWERMANAGER_STATUSBIT_HAS_POWERDOWN);
	powermanager_status &= ~(1<<POWERMANAGER_STATUSBIT_HAS_POWERDOWN);
	return result;
}
uint8_t powermanager_has_new_reference_time()
{
	uint8_t result = powermanager_status & (1<<POWERMANAGER_STATUSBIT_HAS_TIMEREF);
	powermanager_status &= ~(1<<POWERMANAGER_STATUSBIT_HAS_TIMEREF);
	return result;
}
uint8_t powermanager_has_command_reload_config()
{
	uint8_t result = powermanager_status & (1<<POWERMANAGER_STATUSBIT_HAS_RELOADCONFIG);
	powermanager_status &= ~(1<<POWERMANAGER_STATUSBIT_HAS_RELOADCONFIG);
	return result;
}

// misc
uint32_t powermanager_current_time() // in seconds since 0:00:00
{
	return (millis() - last_timeref_millis) / 1000;
}

// power state
uint8_t powermanager_has_power_for_experiment()
{ // minimum battery level and power good
	return (powermanager_get_charge_state() >= POWERMANAGER_EXPERIMENT_MIN_CHARGE) && get_pgood();
}
uint8_t powermanager_has_power_for_small_things()
{	// minimum battery level
	return (powermanager_get_charge_state() >= POWERMANAGER_SMALLTHINGS_MIN_CHARGE) && get_pgood();
}

// power switching
void powermanager_8V_on() {
	digitalWrite(CORE__PIN_DOUT_MOSFET_8V,HIGH);
}

void powermanager_8V_off() {
	digitalWrite(CORE__PIN_DOUT_MOSFET_8V,LOW);	
}

void powermanager_5V_on() {
	digitalWrite(CORE__PIN_DOUT_MOSFET_5V,HIGH);
}

void powermanager_5V_off() {
	digitalWrite(CORE__PIN_DOUT_MOSFET_5V,LOW);	
}
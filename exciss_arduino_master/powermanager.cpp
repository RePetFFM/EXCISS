#include <SparkFunBQ27441.h>
#include "exciss.h"

#define POWERMANAGER_CALL_FREQUENCY 1 // call period (in seconds)

#define POWERMANAGER_N_POWERDOWN 3
#define POWERMANAGER_N_RELOADCONFIG 4
#define POWERMANAGER_N_TIMEREF 2

#define POWERMANAGER_STATUSBIT_HAS_POWERDOWN 0
#define POWERMANAGER_STATUSBIT_HAS_TIMEREF 1
#define POWERMANAGER_STATUSBIT_HAS_RELOADCONFIG 2

#define POWERMANAGER_THERSHOLD_MIN_USB_ON 100
#define POWERMANAGER_THERSHOLD_MAX_USB_OFF 50

// times in milliseconds
//const uint32_t POWERMANAGER_T_OFF_MIN=50*((uint32_t)1000); // minimum length of power low cycle
//const uint32_t POWERMANAGER_T_OFF_MAX=70*((uint32_t)1000); // maximum length of power low cycle
//const uint32_t POWERMANAGER_T_WAIT_MAX=5*60*((uint32_t)1000); // maximum distance of power low cycles
const uint32_t POWERMANAGER_T_OFF_MIN=4*((uint32_t)1000); // minimum length of power low cycle
const uint32_t POWERMANAGER_T_OFF_MAX=8*((uint32_t)1000); // maximum length of power low cycle
const uint32_t POWERMANAGER_T_WAIT_MAX=5*6*((uint32_t)1000); // maximum distance of power low cycles


uint8_t powermanager_status = 0;


uint64_t last_timeref_millis = 0;
uint8_t last_state = 0;
uint64_t last_down_flank = 0;
uint64_t last_cycle_end = 0;
uint8_t cycle_count = 0;

uint8_t get_usb_in_voltage()
{
	return analogRead(CORE__PIN_AIN_USB_POWER_SENSE);
}

inline uint8_t usb_power_is_on()
{
	if(get_usb_in_voltage()>POWERMANAGER_THERSHOLD_MIN_USB_ON )
	return analogRead(CORE__PIN_AIN_USB_POWER_SENSE);
}

bool powermanager_begin()
{
	if(!lipo.begin()) {
		return false;
	} else {
		lipo.setCapacity(POWERMANAGER_CELL_CAPACITY);
		return true;	
	}
	return false;
}

uint16_t powermanager_get_charge_state()
{
	return lipo.soc();
}
void powermanager_poll()
{
	// check power cycles
	uint8_t usb_power_on = get_usb_in_voltage();
	if(usb_power_on != last_state)
	{
		if(usb_power_on) // up flank
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
		last_state = usb_power_on;
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
	return (powermanager_get_charge_state() >= POWERMANAGER_EXPERIMENT_MIN_CHARGE) && get_usb_in_voltage();
}
uint8_t powermanager_has_power_for_small_things()
{	// minimum battery level
	return (powermanager_get_charge_state() >= POWERMANAGER_SMALLTHINGS_MIN_CHARGE) && get_usb_in_voltage();
}

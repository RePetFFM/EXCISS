#include <SparkFunBQ27441.h>
#include "exciss.h"


uint8_t powermanager_status = 0;
uint64_t last_timeref_millis = 0;

uint8_t powermanager_shutdown_request = 0xA5;

inline uint8_t get_pgood()
{
    return digitalRead(CORE__PIN_DOUT_BABYSITTER_PGOOD) == LOW;
}

uint8_t powermanager_begin()
{
    pinMode(CORE__PIN_DOUT_BABYSITTER_PGOOD, INPUT_PULLUP);
    lipo.begin();
    lipo.setCapacity(POWERMANAGER_CELL_CAPACITY);
    return true;
}
uint16_t powermanager_get_charge_state()
{
    return lipo.soc();
}

uint16_t powermanager_get_voltage() {
    return lipo.voltage();
}

uint16_t powermanager_get_capacity() {
    return lipo.capacity(REMAIN);
}

uint8_t powermanager_get_usb_power_status() {
    return get_pgood();
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

void powermanager_vsys_on() {
    digitalWrite(CORE__PIN_DOUT_BABYSITTER_SYSOFF, HIGH);
}

void powermanager_vsys_off() {
    digitalWrite(CORE__PIN_DOUT_BABYSITTER_SYSOFF, LOW);
}

void powermanager_poll_powercycle_command()
{
    static uint32_t usb_power_status = 0;
    static uint32_t previous_usb_power_status = 0;
    static uint32_t last_level_change_millis;
    static uint8_t power_cycle_count = 0;
    static uint8_t power_cycle_valid_command_count = 0;
    static uint32_t next_interval_millis;
    static uint32_t last_power_good_millis;

    uint32_t dt_last_level_change_millis;
    uint8_t pgood = get_pgood();
    

    if(pgood) {
        usb_power_status = (usb_power_status<<1) | 1;
    } else {
        usb_power_status = usb_power_status<<1;
    }

    if((usb_power_status==0xFFFFFFFF || usb_power_status==0) && usb_power_status != previous_usb_power_status)
    {
        previous_usb_power_status = usb_power_status;

        dt_last_level_change_millis = millis() - last_level_change_millis;

        if(power_cycle_count==0 && usb_power_status==0) {
            next_interval_millis = millis() + POWERMANAGER_POWERCYCLE_INTERVAL_MILLIS;    
        }
        

        last_level_change_millis = millis();

        power_cycle_count++;

        Serial.print("PGOOD: ");
        Serial.print(usb_power_status,HEX);
        Serial.print("PC: ");
        Serial.println(power_cycle_count);
    
        if(next_interval_millis>0 && power_cycle_count>1) {
            Serial.println(next_interval_millis);
            if( millis()>next_interval_millis-POWERMANAGER_POWERCYCLE_MARGIN_MILLIS
                && next_interval_millis+POWERMANAGER_POWERCYCLE_MARGIN_MILLIS<millis() ) {
                power_cycle_valid_command_count++;
                Serial.print("valid count");
                Serial.println(power_cycle_valid_command_count);
            }
        }    
        

        if(usb_power_status==0xFFFFFFFF) {
            last_power_good_millis = 0;
        } else {
            last_power_good_millis = millis();
        }
    }

    if(millis()>=next_interval_millis){
        next_interval_millis = millis() + POWERMANAGER_POWERCYCLE_INTERVAL_MILLIS;
    }


    if(last_level_change_millis > 0)
    {
        if(millis()>= (last_level_change_millis+POWERMANAGER_POWERCYCLE_END_DELAY_MILLIS)) // end of command
        {

            Serial.print("max wait time until next command reached ");
            Serial.println(last_level_change_millis);
            if(power_cycle_valid_command_count == POWERMANAGER_N_RELOADCONFIG) {
                Serial.println("RELOAD CONFIG REQUEST");
                powermanager_status |= (1<<POWERMANAGER_STATUSBIT_HAS_RELOADCONFIG);
            }
             
            power_cycle_count = 0;
            last_level_change_millis = 0;
            power_cycle_valid_command_count = 0;
            powermanager_shutdown_request = 0xA5;
        }
    }
    
    if(power_cycle_valid_command_count > POWERMANAGER_N_MAX_CYCLE || power_cycle_count > POWERMANAGER_N_MAX_CYCLE)
    {
        Serial.println("too many cycles");
        power_cycle_count = 0;
        last_level_change_millis = 0;
        power_cycle_valid_command_count = 0;
        powermanager_shutdown_request = 0xA5;
    }

    if(last_power_good_millis>0 && millis()>(last_power_good_millis+POWERMANAGER_FORCE_POWER_DOWN_WAIT_MILLIS)) {
        Serial.println("FORCE POWER DOWN DUE TO USB POWER DOWN");
        power_cycle_count = 0;
        last_level_change_millis = 0;
        powermanager_shutdown_request = 0x5A;
        powermanager_8V_off();
        powermanager_5V_off();
        powermanager_vsys_off();
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

uint8_t powermanager_shutdown_requested() {
    return powermanager_shutdown_request;
}

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
{   // minimum battery level
    return (powermanager_get_charge_state() >= POWERMANAGER_SMALLTHINGS_MIN_CHARGE) && get_pgood();
}


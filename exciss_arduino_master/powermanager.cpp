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
    static uint32_t last_power_good_millis = 0UL;
    static uint32_t usb_power_status = 0;
    static uint32_t previous_usb_power_status = 0;    
    static uint8_t power_cycle_valid_command_count = 0;
    static uint32_t powermanager_powercycle_timeout_millis;
    static uint32_t last_power_change_millis;

    get_pgood() ? usb_power_status = (usb_power_status<<1) | 1 : usb_power_status = usb_power_status<<1;

    
    if((usb_power_status==0xFFFFFFFF || usb_power_status==0) && usb_power_status != previous_usb_power_status)
    {
        previous_usb_power_status = usb_power_status;

        if(powermanager_powercycle_timeout_millis==0 && usb_power_status==0xFFFFFFFF) {
            Serial.println("Start cycle");
            powermanager_powercycle_timeout_millis = millis() + POWERMANAGER_POWERCYCLE_WINDOW_MILLIS;
            power_cycle_valid_command_count++;
            Serial.println(powermanager_powercycle_timeout_millis);
            Serial.println(millis());
        }
        
        
        Serial.print("PGOOD: ");
        Serial.println(usb_power_status,HEX);
    
        if(last_power_change_millis>0 && power_cycle_valid_command_count>0) {
            if( millis()>last_power_change_millis+POWERMANAGER_POWERCYCLE_MIN_INTERVAL) {
                power_cycle_valid_command_count++;
                Serial.print("valid count");
                Serial.println(power_cycle_valid_command_count);
            }
        }

        last_power_change_millis = millis();
        

        if(usb_power_status==0xFFFFFFFF) {
            last_power_good_millis = 0UL;
        } else {
            last_power_good_millis = millis();
        }
    }

    if(powermanager_powercycle_timeout_millis>0 && millis()>powermanager_powercycle_timeout_millis)
    {
        Serial.println("powercommand max wait time");
        if(power_cycle_valid_command_count == POWERMANAGER_N_RELOADCONFIG) {
            Serial.println("RELOAD CONFIG REQUEST");
            powermanager_status |= (1<<POWERMANAGER_STATUSBIT_HAS_RELOADCONFIG);
        }
        
        last_power_change_millis = 0UL;
        powermanager_powercycle_timeout_millis = 0UL;
        power_cycle_valid_command_count = 0;
    } 

    if(last_power_good_millis>0UL && millis()>(last_power_good_millis+POWERMANAGER_FORCE_POWER_DOWN_WAIT_MILLIS)) {
        Serial.println("FORCE POWER DOWN DUE TO USB POWER DOWN");
        last_power_change_millis = 0UL;
        powermanager_powercycle_timeout_millis = 0UL;
        power_cycle_valid_command_count = 0;
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


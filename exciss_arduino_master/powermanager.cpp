#include <SparkFunBQ27441.h>
#include "exciss.h"


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
void powermanager_11V_on() {
    digitalWrite(CORE__PIN_DOUT_MOSFET_11V,HIGH);
}

void powermanager_11V_off() {
    digitalWrite(CORE__PIN_DOUT_MOSFET_11V,LOW); 
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
    

    get_pgood() ? usb_power_status = (usb_power_status<<1) | 1 : usb_power_status = usb_power_status<<1;

    
    if((usb_power_status==0xFFFFFFFF || usb_power_status==0) && usb_power_status != previous_usb_power_status)
    {
        previous_usb_power_status = usb_power_status;
    
        Serial.print("PGOOD: ");
        Serial.println(usb_power_status,HEX);
    
        if(usb_power_status==0xFFFFFFFF) {
            last_power_good_millis = 0UL;
        } else {
            last_power_good_millis = millis();
        }
    }


    if(powermanager_shutdown_request==0xA5 && last_power_good_millis>0UL && millis()>(last_power_good_millis+POWERMANAGER_FORCE_POWER_DOWN_WAIT_MILLIS)) {
        Serial.println("FORCE POWER DOWN DUE TO USB POWER DOWN ");
        powermanager_shutdown_request = 0x5A;
    }
}



uint8_t powermanager_shutdown_requested() {
    return powermanager_shutdown_request;
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


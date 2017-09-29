#include "exciss.h"

uint8_t powermanager_status = 0;
uint64_t last_timeref_millis = 0;

uint8_t powermanager_shutdown_request = 0xA5;


void setup() {
	pinMode(11, INPUT_PULLUP); // Btn 11
	pinMode(10, INPUT_PULLUP); // Btn 10

	pinMode(7, OUTPUT); // LED 7
	pinMode(8, OUTPUT); // LED 8
	Serial.begin(9600);

	Serial.println("start");

    uint32_t test = POWERMANAGER_FORCE_POWER_DOWN_WAIT_MILLIS;

    Serial.println(test);

    // 3600000
    // 3600000

}

void loop() {
	powermanager_poll_powercycle_command();
}


uint8_t get_pgood() {
	return digitalRead(10)==HIGH;
}

void powermanager_poll_powercycle_command()
{
    static uint32_t last_power_good_millis = 0UL;

    static uint32_t usb_power_status = 0;
    static uint32_t previous_usb_power_status = 0;
    
    static uint8_t power_cycle_valid_command_count = 0;

    static uint32_t powermanager_powercycle_timeout_millis;
    static uint32_t test;

    static uint32_t last_power_change_millis;

    get_pgood() ? usb_power_status = (usb_power_status<<1) | 1 : usb_power_status = usb_power_status<<1;

	
    if((usb_power_status==0xFFFFFFFF || usb_power_status==0) && usb_power_status != previous_usb_power_status)
    {
    	previous_usb_power_status = usb_power_status;

        if(powermanager_powercycle_timeout_millis==0) {
        	Serial.println("Start cycle");
        	powermanager_powercycle_timeout_millis = millis() + POWERMANAGER_POWERCYCLE_WINDOW_MILLIS;
        	power_cycle_valid_command_count++;
        	test = POWERMANAGER_POWERCYCLE_WINDOW_MILLIS;
        	Serial.println(test);
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
        // powermanager_shutdown_request = 0xA5;
    
    } 

    if(last_power_good_millis>0UL && millis()>(last_power_good_millis+POWERMANAGER_FORCE_POWER_DOWN_WAIT_MILLIS)) {
        Serial.println("FORCE POWER DOWN DUE TO USB POWER DOWN");
        last_power_change_millis = 0UL;
        powermanager_powercycle_timeout_millis = 0UL;
        power_cycle_valid_command_count = 0;
        // powermanager_8V_off();
        // powermanager_5V_off();
        // powermanager_vsys_off();
    }
}
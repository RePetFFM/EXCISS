/***********************************************************************
* FILENAME :        hvu.cpp             DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
*       library for high voltage unit periferial control.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    18 Feb 2018
*
**/


#include <Arduino.h>
#include "config.h"
#include "portmap.h"
#include "statemachine_definitions.h"

int HVU__target_voltage = 0;

int HVU__last_target_voltage;
int HVU__last_actual_voltage;
int HVU__last_after_ignition_voltage;

uint8_t SM_HVU__state = SM_HVU__STATE_OFF;
uint8_t SM_HVU__next_state;


uint32_t SM_HVU__delay = 0;

bool ingition_requested =  false;







int HVU__get_charge()
{
	uint16_t current_cap_voltage_raw_analog = analogRead(AIN_HVU_MEASURE);
	uint16_t current_cap_voltage = map(current_cap_voltage_raw_analog, 0, 1023, 0, HVU_VOLTAGE_MAX_CHARGE);
	
	return current_cap_voltage;
}

void HVU__start_charging(int voltage)
{
	HVU__target_voltage = voltage;
	digitalWrite(DOUT_HVU_ENABLE, HVU_ENABLE);
}

void HVU__stop_charging()
{
	digitalWrite(DOUT_HVU_ENABLE, HVU_DISABLE);
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

	digitalWrite( DOUT_HVU_ENABLE , HVU_DISABLE );
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



//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: HVU statemachine


uint8_t ignition_fail_counter = 0;

int ignition_requested_charging_voltage = 0;
int ignition_charging_capacity_target_voltage = 0;
uint32_t HVU__ignition_sm_delay;
uint32_t HVU__ignition_sm_delay_next_state;

void HVU__statemachine() {

	/*
    static unsigned long ignition_max_charging_runtime; 
    static unsigned long ignition_retry_delay;
    


    static uint32_t laststate = 0;
    if(laststate!=SM_HVU__state) {
        laststate = SM_HVU__state;
        Serial.print("sm");
        Serial.println(SM_HVU__state);
    }

    if(SM_HVU__delay>0 && millis()>SM_HVU__delay) {
        SM_HVU__delay = 0;
        SM_HVU__state = SM_HVU__next_state;
        SM_HVU__next_state = 0;
    }
    
    switch (SM_HVU__state) {

        case SM_HVU__STATE_OFF:
            ignition_requested_charging_voltage = 0;
            ignition_charging_capacity_target_voltage = 0;
            
            ingition_requested = false;
        break;

        case SM_HVU__STATE_IDLE:
            // wait state for charg reques from raspberry pi
            if(ignition_requested_charging_voltage>=HVU_MIN_CHARGE_VALUE) {
                ignition_charging_capacity_target_voltage = ignition_requested_charging_voltage;
                if(ignition_charging_capacity_target_voltage>0) {
                    HVU__start_charging(ignition_charging_capacity_target_voltage);    
                    ignition_max_charging_runtime = millis() + CORE_IGNITION_MAX_CHARGTIME_MILLIS; // 300 second max charging time
                    
                    SM_HVU__state = SM_HVU__STATE_CHARGING;
                } else {
                    // if cap charg voltage target is 0 (init default value) abort ignition
                    // send failure code to raspberry pi
                    
                    HVU__abort();
                    ignition_failure_code = 11;
                    SM_HVU__state = SM_HVU__STATE_IGNITION_FAIL;
                }
            } else if(ignition_requested_charging_voltage==-1) {
                ignition_charging_capacity_target_voltage = 0;
                ignition_failure_code = 1;
                SM_HVU__state = SM_HVU__STATE_IGNITION_FAIL;
            }
        
        break;

        
        case SM_HVU__STATE_CHARGING:
            // charging arc capacitor until requested voltage value
            
            // Serial.println(chargemonitor_get_charge());
            if(HVU__has_enough_charge()) {
                HVU__stop_charging();
                SM_HVU__state = CORE__IGNITION_SM_T_IGNITION_READY;
                ignition_fail_counter = 0;
                
            }

            if(millis()>=ignition_max_charging_runtime) {
                // max chargingtime was exceeded abort charging
                HVU__abort();
                
                ignition_failure_code = 12;
                SM_HVU__state = SM_HVU__STATE_IGNITION_FAIL;
            }
        break;

        case CORE__IGNITION_SM_T_IGNITION_READY:
            if(ingition_requested) {
                SM_HVU__state = SM_HVU__STATE_IGNITE; 
            }
        break;

        case SM_HVU__STATE_IGNITE:
            HVU__ignite();
            Serial.print("IG");
            Serial.print(chargemonitor_last_actual_voltage);
            Serial.print(" ");
            Serial.print(chargemonitor_last_after_ignition_voltage);
            SM_HVU__state = CORE__IGNITION_SM_L_IGNITION_REDO;
        break;

        case CORE__IGNITION_SM_L_IGNITION_REDO:
            if(!HVU__last_ignition_succeeded() && ignition_fail_counter<CORE_IGNITION_RETRY_MAX_COUNT) {
                // the ignition spark doesn't work or was not sucsessful to trigger the discharg arc.
                // reignite th ignition spark and retry.
                
                ignition_fail_counter++;
                Serial.print("IG_F"); // F = fail
                Serial.println(ignition_fail_counter);

                SM_HVU__delay = millis()+CORE_IGNITION_RETRY_DELAY_MILLIS;
                SM_HVU__next_state = SM_HVU__STATE_CHARGING;
                ingition_requested = false;
            } else if(ignition_fail_counter==0) {
                Serial.println("IG_GOOD"); // G = good
                
                SM_HVU__state = SM_HVU__STATE_OFF;
            } else {
                ignition_failure_code = 21;
                SM_HVU__state = SM_HVU__STATE_IGNITION_FAIL;
            }
        break;

        case SM_HVU__STATE_IGNITION_FAIL:
            // ignition failure handling
            // ignition_armed = 0;
            ingition_requested = false;
            
            Serial.print("IGF ");
            Serial.println(ignition_failure_code,HEX);
            SM_HVU__state = SM_HVU__STATE_OFF;
        break;

        default:

        break;
    }

    */
    
}

// end: HVU statemachine
//..............................................................
//..............................................................





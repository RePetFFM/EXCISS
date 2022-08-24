/***********************************************************************
* FILENAME :        hvu.cpp             DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* This is the main code for HVU (high voltage unit) control routines.
* By calling specific interface functions, the target voltage for pre-charge
* and ignition is defined. 
* The main hvu state machine is responsible to ensure correct execution order
* and safe operation by aborting charging if timeout-condition or unsafe voltage
* is reched. 
*
* All default configurations can be found in the config_hvu.h header file.
* 
* AUTHOR :    Shintaro Fujita
*
* CONFIG FILES: config_hvu.h
*
**/


#include "HVU.h"
#include "exciss.h"
#include "config_hvu.h"


int         _hvu__target_voltage    = 0;

int         _hvu__ignition_voltage  = 0;

int         _hvu__ignition_cap_voltage_before   = 0;

int         _hvu__ignition_cap_voltage_after    = 0;

int         _hvu__ignition_delta_threshold      = HVU__IGNITION_DELTA;

uint32_t    _hvu__ignition_time                 = 0x0;



uint8_t     _huv__ignition_failure_code = 0x0;

uint8_t     _hvu__abort_error_code      = 0x00;

uint32_t    _hvu__request_state_change  = 0x0;

uint32_t    _hvu__sm_state_next         = HVU__SM_STATE_OFF;

uint32_t    _hvu__sm_state              = HVU__SM_STATE_OFF;





void HVU__init() 
{
    // should be call by the root init function.
    // reset all hvu control pin's  to safe default state.

    pinMode( DOUT_HVU_ENABLE , OUTPUT );
    digitalWrite( DOUT_HVU_ENABLE , STATE_HVU_DISABLE );

    pinMode( DOUT_HVU_TRIGGER , OUTPUT );
    digitalWrite( DOUT_HVU_TRIGGER , STATE_HVU_TRIGGER_DISABLE );
}


void HVU__enable() 
{
    com.log("[ HVU ]");
    com.newline();
    com.log("HVU ENABLED");
    com.newline();

    pinMode( DOUT_HVU_ENABLE , OUTPUT );
    digitalWrite( DOUT_HVU_ENABLE , STATE_HVU_ENABLE );
}


void HVU__disable() 
{
    // disables HVU, but doesn't do a quick discharge.
    // To do a quick discharge, the vsys must be disabled, 
    // but disabling vsys disables also front&back LED and haptic driver's power sources.

    digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );
    digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);
    digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_DISABLE);
    
    com.log("[ HVU ]");
    com.newline();
    com.log("HVU DISABLED");
    com.newline();
}


int HVU__get_charge()
{
    // read capacitor voltage. The raw data is in range of 0 - 1023, or 0 - 3.3 volts.
    uint16_t current_cap_voltage_raw_analog = analogRead(AIN_HVU_MEASURE);
    
    // convert raw analog value to the real capcitor voltage
    float analog_input_voltage = (3.3 / 1023.0) * (float)current_cap_voltage_raw_analog;
    float cap_voltage = 197.91 * analog_input_voltage;

    return (int)cap_voltage;
}


void HVU__set_ignition_delta(int a_voltage)
{
    // setter to change the ignition voltage threshold.
    // It sets the delta voltage, both negativ and positiv, to the target voltage.

    com.log("[ HVU ]");
    com.newline();
    
    if( a_voltage>=0 && a_voltage<=30 ) 
    {
        com.log("ignition_delta = ");
        com.log( a_voltage );
        _hvu__ignition_delta_threshold = a_voltage;
    } 
    else
    {
        // out of allowed voltage range
        com.log("error = out of allowed voltage range 0-30");
    }

    com.newline();
}


void HVU__stop_charging()
{
    digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE );
}


void HVU__abort()
{
    POWER__mode_end_science_experiment(); // turns off vsys to initiate a quick discharge.

    HVU__disable();
    
    _hvu__target_voltage = 0;
    _hvu__ignition_voltage = 0;

    _hvu__sm_state_next = HVU__SM_STATE_ABORT;

    com.log("[ HVU ]");
    com.newline();
    com.log("CHARG ABORTED");
    com.newline();
}


bool HVU__has_enough_charge()
{
    return HVU__get_charge() >= _hvu__target_voltage;
}


void HVU__ignite()
{
    // this is the main ignition routine.


    // enable hvu to enable ignition module
    digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_ENABLE);

    delay(100); // ignition module require some time to work

    // get charge voltage before ignition
    _hvu__ignition_cap_voltage_before = HVU__get_charge();


    // store the ignition time
    _hvu__ignition_time = millis();

    digitalWrite( DOUT_HVU_TRIGGER , STATE_HVU_TRIGGER_ENABLE );
    delay( HVU__IGNITE_TOGGLE_DURATION );
    digitalWrite( DOUT_HVU_TRIGGER , STATE_HVU_TRIGGER_DISABLE );
    delay( HVU__MEASURE_WAIT_AFTER_IGNITION );

    // get charge voltage after ignition
    _hvu__ignition_cap_voltage_after = HVU__get_charge();


    com.log("[ HVU ]");
    com.newline();

    // check ignition 
    if( _hvu__ignition_cap_voltage_after <= HVU__DISCHARGE_SUCCESS_THRESHOLD ) 
    {
        _huv__ignition_failure_code = HVU__STATUS_CODE_IGNITION_SUCCESSFUL;
        com.log("ignition_result = SUCCESSFUL");
        com.newline();
    } 
    else 
    {
        _huv__ignition_failure_code = HVU__STATUS_CODE_ERROR_IGNITION_FAILED_THRESHOLD;
        com.log("ignition_result = FAILED_0xE1");
        com.newline();
    }

    // charge
    com.log("charge_voltage_before_ignition = ");
    com.log(_hvu__ignition_cap_voltage_before);
    com.newline();   
    
    com.log("charge_voltage_after_ignition = ");
    com.log(_hvu__ignition_cap_voltage_after);
    com.newline();
}


void HVU__set_charg_target(int a_voltage) 
{
    com.log("[ HVU ]");
    com.newline();
    com.log("charg_target_voltage = ");
    com.log(a_voltage);
    com.newline();

    _hvu__target_voltage = a_voltage;
}


void HVU__start_charging(int a_voltage) 
{
    _hvu__request_state_change = HVU__SM_STATE_CHARGING;

    HVU__set_charg_target(a_voltage);
}


void HVU__ignite_at_charg(int a_voltage) 
{
    com.log("[ HVU ]");
    com.newline();
    com.log("igniton_target_voltage = ");
    com.log(a_voltage);
    com.newline();

    _hvu__ignition_voltage = a_voltage;

    _hvu__request_state_change = HVU__SM_STATE_IGNITION_REQUESTED;

    HVU__set_charg_target(a_voltage);
}


void HVU__do_test_ignition_without_arc()
{
    // this function should be used to do a test ignition without arc.
    // to prevent arcing, the capacitor voltage is checked before ignition.

    com.log("[ HVU ]");
    com.newline();
    com.log("request test igntion without arc");
    com.newline();


    if( HVU__get_charge() < HVU__TEST_IGNITION_CONDITION_MAX_CAP_VOLTAGE )
    {
        digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

        digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_ENABLE);

        delay(1000);
        
        digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_ENABLE);
        delay(1);
        digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_DISABLE);

        digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);

        digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );

        com.log("result = done");
        com.newline();    
    } 
    else 
    {
        com.log("result = error: cap voltage too high");
        com.newline();
        com.log("cap_voltage = ");
        com.log( HVU__get_charge() );
        com.newline();
    }

    
}


void HVU__logout_status() {
    com.log("[ HVU ]");
    com.newline();

    com.log("hvu_state = ");

    switch(_hvu__sm_state) {
        case HVU__SM_STATE_OFF:
            com.log("off");
        break;

        case HVU__SM_STATE_STANDBY:
            com.log("standby");
        break;

        case HVU__SM_STATE_CHARGING:
            com.log("charging");
        break;

        case HVU__SM_STATE_CHARGED:
            com.log("charged");
        break;

        case HVU__SM_STATE_IGNITION_REQUESTED:
            com.log("wait_ignition");
        break;

        case HVU__SM_STATE_ABORT:
            com.log("aborted");
        break;

        case HVU__SM_STATE_ABORT_WITH_ERROR:
            com.log("failed");
        break;

        default:
        break;

    }
    com.newline();

    com.log("cap_voltage = ");
    com.log( HVU__get_charge() );
    com.newline();    

    com.log("igniton_target_voltage = ");
    com.log(_hvu__ignition_voltage);
    com.newline();    

    com.log("charg_target_voltage = ");
    com.log(_hvu__target_voltage);
    com.newline();


    // last igntion results
    com.log("last_ignition_result = ");
    com.logHEX( _huv__ignition_failure_code );
    com.newline();

    com.log("last_ignition_time = ");
    com.log_uint32_t( _hvu__ignition_time );
    com.newline();

    com.log("charge_voltage_before_ignition = ");
    com.log(_hvu__ignition_cap_voltage_before);
    com.newline();   
    
    com.log("charge_voltage_after_ignition = ");
    com.log(_hvu__ignition_cap_voltage_after);
    com.newline();    

}


void HVU__loop() 
{
    
    
    static uint32_t _hvu__sm_state_last = HVU__SM_STATE_OFF;

    // timekeepers
    static unsigned long _hvu__timeout_charging_start = 0UL;
    static unsigned long _hvu__timeout_charging_delay = 0UL;
    static unsigned long _hvu__timeout_charg_hold_start = 0UL;
    static unsigned long _hvu__timeout_charg_hold_delay = 0UL;
    static unsigned long _hvu__timeout_charg_hold_wait_next_charg_start = 0UL;
    static unsigned long _hvu__timeout_charg_hold_wait_next_charg_delay = 0UL;
    static unsigned long _hvu__timeout_ignition_charging_start = 0UL;
    static unsigned long _hvu__timeout_ignition_charging_delay = 0UL;

    int delta_target;


    if( _hvu__sm_state != _hvu__sm_state_next ) 
    {

        com.log("[ HVU ]");
        com.newline();
        com.log("sm_change_hvu = ");
        com.logHEX(_hvu__sm_state_last);
        com.log("->");
        com.logHEX(_hvu__sm_state_next);
        com.newline();

        _hvu__sm_state = _hvu__sm_state_next;
    }

    switch(_hvu__sm_state) {
        case HVU__SM_STATE_OFF:
            if( POWER__science_power_enabled()==0xAA ) 
            {
                // science power enabled, change state to standby and wait for commands
                _hvu__sm_state_next = HVU__SM_STATE_STANDBY;
            }

            // eneble hvu enable pin to enable charging
            digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );
            digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);
            digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_DISABLE);

            _hvu__ignition_time = 0UL;
            _huv__ignition_failure_code = 0x0;
            _hvu__target_voltage = 0;
            _hvu__ignition_voltage = 0;
            
        break;

        case HVU__SM_STATE_STANDBY:
            // wait for charg voltage request

            if( _hvu__sm_state_last != HVU__SM_STATE_STANDBY ) 
            {
                digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );
                digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);
                digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_DISABLE);

                
            }

            if( _hvu__request_state_change == HVU__SM_STATE_CHARGING ) 
            {
                _hvu__sm_state_next = HVU__SM_STATE_CHARGING;

                // enable hvu
                digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

                // resset last ignition results
                _hvu__ignition_time                 = 0UL;
                _huv__ignition_failure_code         = 0x0;
                _hvu__ignition_cap_voltage_before   = 0;
                _hvu__ignition_cap_voltage_after    = 0;
            }

            // allow switch mode to ignition request
            if( _hvu__request_state_change == HVU__SM_STATE_IGNITION_REQUESTED ) 
            {
                _hvu__sm_state_next = HVU__SM_STATE_IGNITION_REQUESTED;

                // enable hvu
                digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

                // resset last ignition results
                _hvu__ignition_time                 = 0UL;
                _huv__ignition_failure_code         = 0x0;
                _hvu__ignition_cap_voltage_before   = 0;
                _hvu__ignition_cap_voltage_after    = 0;
            }

            
        break;

        case HVU__SM_STATE_CHARGING:

            if( _hvu__sm_state_last == HVU__SM_STATE_STANDBY || _hvu__sm_state_last == HVU__SM_STATE_CHARGED ) 
            {
                digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

                // resset charging start timestamp
                _hvu__timeout_charging_start = millis(); 
                _hvu__timeout_charging_delay = HVU__CHARGING_MAX_TIME_UNTIL_ABORT_SEC * 1000UL;
            }



            if( HVU__has_enough_charge() ) 
            {
                // requested charg level reached, switch to state charged and hold charg
                _hvu__sm_state_next = HVU__SM_STATE_CHARGED;    
            }

            // eneble hvu enable pin to enable charging
            digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_ENABLE);


            // allow switch mode to ignition request
            if( _hvu__request_state_change == HVU__SM_STATE_IGNITION_REQUESTED ) 
            {
                _hvu__sm_state_next = HVU__SM_STATE_IGNITION_REQUESTED;
            }

            // ----------------------
            // charg abort conditions
            // 1. charg time exceeds the maximumg wait time
            if( millis() - _hvu__timeout_charging_start >= _hvu__timeout_charging_delay ) 
            {
                _hvu__abort_error_code = HVU__STATUS_CODE_ERROR_CHARG_TIMEOUT;

                _hvu__sm_state_next = HVU__SM_STATE_ABORT_WITH_ERROR;
            }

            // 2. charg voltage overshoots
            if( HVU__get_charge() >= HVU__CAPACITOR_MAX_VOLTAGE_VALUE )
            {
                _hvu__abort_error_code = HVU__STATUS_CODE_ERROR_CHARG_OVERSHOOTED;

                _hvu__sm_state_next = HVU__SM_STATE_ABORT_WITH_ERROR;
            }
        break;

        case HVU__SM_STATE_CHARGED:
            if( _hvu__sm_state_last == HVU__SM_STATE_CHARGING ) 
            {
                 // resset charg hold start timestamp
                _hvu__timeout_charg_hold_start = millis();
                _hvu__timeout_charg_hold_delay = (HVU__CHARGED_MAX_WAIT_TIME_IGNITION_REQUEST_UNTIL_ABORT_SEC * 1000UL);
            }

            // ----------------------------------
            // begin: charg hold control
            if( HVU__has_enough_charge() ) 
            {
                digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);

                _hvu__timeout_charg_hold_wait_next_charg_start = millis();
                _hvu__timeout_charg_hold_wait_next_charg_delay = 250UL;

            } 

            if( !HVU__has_enough_charge() && ( millis() - _hvu__timeout_charg_hold_wait_next_charg_start >= _hvu__timeout_charg_hold_wait_next_charg_delay) )
            {
                digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_ENABLE);
            }
            // end: charg hold control
            // ----------------------------------
            


            // allow switch mode to ignition request
            if( _hvu__request_state_change == HVU__SM_STATE_IGNITION_REQUESTED ) 
            {
                _hvu__sm_state_next = HVU__SM_STATE_IGNITION_REQUESTED;
            }


            // allow switch mode to charge request
            if( _hvu__request_state_change == HVU__SM_STATE_CHARGING ) 
            {
                _hvu__sm_state_next = HVU__SM_STATE_CHARGING;
            }
            

            // ----------------------
            // charg hold  abort conditions
            // 1. charg time exceeds the maximumg wait time
            if( millis() - _hvu__timeout_charg_hold_start >= _hvu__timeout_charg_hold_delay ) 
            {
                _hvu__abort_error_code = HVU__STATUS_CODE_ERROR_CHARG_HOLD_TIMEOUT;

                _hvu__sm_state_next = HVU__SM_STATE_ABORT_WITH_ERROR;
            }

            // 2. charg voltage overshoots
            if( HVU__get_charge() >= HVU__CAPACITOR_MAX_VOLTAGE_VALUE )
            {
                _hvu__abort_error_code = HVU__STATUS_CODE_ERROR_CHARG_HOLD_OVERSHOOTED;

                _hvu__sm_state_next = HVU__SM_STATE_ABORT_WITH_ERROR;
            }
        break;

        case HVU__SM_STATE_IGNITION_REQUESTED:
            if( _hvu__sm_state_last != HVU__SM_STATE_IGNITION_REQUESTED ) 
            {
                digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

                // resset charg hold start timestamp
                _hvu__timeout_ignition_charging_start = millis(); 
                _hvu__timeout_ignition_charging_delay = (HVU__IGNITION_MAX_WAIT_TIME_IGNITION_CHARG_UNTIL_ABORT_SEC * 1000UL);
            }

            if( HVU__get_charge() < _hvu__ignition_voltage ) 
            {
                digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_ENABLE);   
            }

            if( HVU__get_charge() > _hvu__ignition_voltage ) 
            {
                digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);   
            }

            // check ignition condition
            delta_target = abs( _hvu__ignition_voltage - HVU__get_charge() );

            if( delta_target < _hvu__ignition_delta_threshold ) 
            {
                // ignition voltage reached. execute ignition.
                HVU__ignite();

                _hvu__target_voltage    = 0;
                _hvu__ignition_voltage  = 0;

                _hvu__sm_state_next = HVU__SM_STATE_STANDBY;   
            }

            // ----------------------
            // charg hold  abort conditions
            // 1. charg time exceeds the maximumg wait time
            if( millis() - _hvu__timeout_ignition_charging_start >= _hvu__timeout_ignition_charging_delay ) 
            {
                _hvu__abort_error_code = HVU__STATUS_CODE_ERROR_IGNITION_CHARG_TIMEOUT;

                _hvu__sm_state_next = HVU__SM_STATE_ABORT_WITH_ERROR;
            }

            // 2. charg voltage overshoots
            if( HVU__get_charge() >= HVU__CAPACITOR_MAX_VOLTAGE_VALUE )
            {
                _hvu__abort_error_code = HVU__STATUS_CODE_ERROR_IGNITION_OVERSHOOTED;

                _hvu__sm_state_next = HVU__SM_STATE_ABORT_WITH_ERROR;
            }
        break;

        case HVU__SM_STATE_ABORT_WITH_ERROR:
            _hvu__sm_state_next = HVU__SM_STATE_STANDBY;

            // disable all ports regarding charging
            digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_DISABLE);
            digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);
            
            POWER__mode_end_science_experiment();

            com.log("[ HVU ]");
            com.newline();
            com.log("aborted");
            com.newline();
            com.log("hvu_abort_error_code = ");
            com.log(_hvu__abort_error_code);
            com.newline();

            _hvu__target_voltage    = 0;
            _hvu__ignition_voltage  = 0;
        break;

        case HVU__SM_STATE_ABORT:
            _hvu__sm_state_next = HVU__SM_STATE_STANDBY;

            // disable all ports regarding charging
            digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );
            digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);
            digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_DISABLE);

            com.log("[ HVU ]");
            com.newline();
            com.log("aborted");
            com.newline();

            _hvu__target_voltage    = 0;
            _hvu__ignition_voltage  = 0;
        break;

        default:
            _hvu__sm_state_next = HVU__SM_STATE_OFF;        
            _hvu__target_voltage    = 0;
            _hvu__ignition_voltage  = 0;
        break;
    }

    if( POWER__science_power_enabled()==0x55 ) 
    {
        // science power was disabled change state to hvu off
        _hvu__sm_state_next = HVU__SM_STATE_OFF;
    }

    _hvu__sm_state_last = _hvu__sm_state;

    _hvu__request_state_change = 0x0;
}

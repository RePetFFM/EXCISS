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
#include "config_hvu.h"
#include "portmap.h"



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
    Serial.print("[ HVU ]");
    Serial.println();
    Serial.print("HVU ENABLED");
    Serial.println();

    pinMode( DOUT_HVU_ENABLE , OUTPUT );
    digitalWrite( DOUT_HVU_ENABLE , STATE_HVU_ENABLE );
}


void HVU__disable() 
{
    // disables HVU, but doesn't do a quick discharge.
    // To do a quick discharge, the vsys must be disabled, 
    // but disabling vsys disables also front&back LED and haptic driver's power sources.

    // digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );
    digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);
    digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_DISABLE);
    
    Serial.print("[ HVU ]");
    Serial.println();
    Serial.print("HVU DISABLED");
    Serial.println();
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


int HVU__get_ignition_before_voltage()
{
    return  _hvu__ignition_cap_voltage_before;
}

void HVU__set_ignition_delta(int a_voltage)
{
    // setter to change the ignition voltage threshold.
    // It sets the delta voltage, both negativ and positiv, to the target voltage.

    Serial.print("[ HVU ]");
    Serial.println();
    
    if( a_voltage>=0 && a_voltage<=30 ) 
    {
        Serial.print("ignition_delta = ");
        Serial.print( a_voltage );
        _hvu__ignition_delta_threshold = a_voltage;
    } 
    else
    {
        // out of allowed voltage range
        Serial.print("error = out of allowed voltage range 0-30");
    }

    Serial.println();
}


void HVU__stop_charging()
{
    digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE );
}


void HVU__abort()
{

    HVU__disable();
    
    _hvu__target_voltage = 0;
    _hvu__ignition_voltage = 0;

    _hvu__sm_state_next = HVU__SM_STATE_ABORT;

    Serial.print("[ HVU ]");
    Serial.println();
    Serial.print("CHARG ABORTED");
    Serial.println();
}


bool HVU__has_enough_charge()
{
    return HVU__get_charge() >= _hvu__target_voltage;
}


uint32_t HVU__get_state()
{
    return _hvu__sm_state;
}

uint32_t HVU__get_last_ignition()
{
    return _huv__ignition_failure_code;
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


    Serial.print("[ HVU ]");
    Serial.println();

    // check ignition 
    if( _hvu__ignition_cap_voltage_after <= HVU__DISCHARGE_SUCCESS_THRESHOLD ) 
    {
        _huv__ignition_failure_code = HVU__STATUS_CODE_IGNITION_SUCCESSFUL;
        Serial.print("ignition_result = SUCCESSFUL");
        Serial.println();
    } 
    else 
    {
        _huv__ignition_failure_code = HVU__STATUS_CODE_ERROR_IGNITION_FAILED_THRESHOLD;
        Serial.print("ignition_result = FAILED_0xE1");
        Serial.println();
    }

    // charge
    Serial.print("charge_voltage_before_ignition = ");
    Serial.print(_hvu__ignition_cap_voltage_before);
    Serial.println();   
    
    Serial.print("charge_voltage_after_ignition = ");
    Serial.print(_hvu__ignition_cap_voltage_after);
    Serial.println();
}


void HVU__set_charg_target(int a_voltage) 
{
    Serial.print("[ HVU ]");
    Serial.println();
    Serial.print("charg_target_voltage = ");
    Serial.print(a_voltage);
    Serial.println();

    _hvu__target_voltage = a_voltage;
}


void HVU__start_charging(int a_voltage) 
{
    _hvu__request_state_change = HVU__SM_STATE_CHARGING;

    HVU__set_charg_target(a_voltage);
}


void HVU__ignite_at_charg(int a_voltage) 
{
    Serial.print("[ HVU ]");
    Serial.println();
    Serial.print("igniton_target_voltage = ");
    Serial.print(a_voltage);
    Serial.println();

    _hvu__ignition_voltage = a_voltage;

    _hvu__request_state_change = HVU__SM_STATE_IGNITION_REQUESTED;

    HVU__set_charg_target(a_voltage);
}


void HVU__do_test_ignition_without_arc()
{
    // this function should be used to do a test ignition without arc.
    // to prevent arcing, the capacitor voltage is checked before ignition.

    Serial.print("[ HVU ]");
    Serial.println();
    Serial.print("request test igntion without arc");
    Serial.println();


    if( HVU__get_charge() < HVU__TEST_IGNITION_CONDITION_MAX_CAP_VOLTAGE )
    {
        // digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

        digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_ENABLE);

        delay(1000);
        
        digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_ENABLE);
        delay(1);
        digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_DISABLE);

        digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);

        // digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );

        Serial.print("result = done");
        Serial.println();    
    } 
    else 
    {
        Serial.print("result = error: cap voltage too high");
        Serial.println();
        Serial.print("cap_voltage = ");
        Serial.print( HVU__get_charge() );
        Serial.println();
    }

    
}


void HVU__logout_status() {
    Serial.print("[ HVU ]");
    Serial.println();

    Serial.print("hvu_state = ");

    switch(_hvu__sm_state) {
        case HVU__SM_STATE_OFF:
            Serial.print("off");
        break;

        case HVU__SM_STATE_STANDBY:
            Serial.print("standby");
        break;

        case HVU__SM_STATE_CHARGING:
            Serial.print("charging");
        break;

        case HVU__SM_STATE_CHARGED:
            Serial.print("charged");
        break;

        case HVU__SM_STATE_IGNITION_REQUESTED:
            Serial.print("wait_ignition");
        break;

        case HVU__SM_STATE_ABORT:
            Serial.print("aborted");
        break;

        case HVU__SM_STATE_ABORT_WITH_ERROR:
            Serial.print("failed");
        break;

        default:
        break;

    }
    Serial.println();

    Serial.print("cap_voltage = ");
    Serial.print( HVU__get_charge() );
    Serial.println();    

    Serial.print("igniton_target_voltage = ");
    Serial.print(_hvu__ignition_voltage);
    Serial.println();    

    Serial.print("charg_target_voltage = ");
    Serial.print(_hvu__target_voltage);
    Serial.println();


    // last igntion results
    Serial.print("last_ignition_result = ");
    Serial.print( _huv__ignition_failure_code ,HEX);
    Serial.println();

    Serial.print("last_ignition_time = ");
    Serial.print( _hvu__ignition_time );
    Serial.println();

    Serial.print("charge_voltage_before_ignition = ");
    Serial.print(_hvu__ignition_cap_voltage_before);
    Serial.println();   
    
    Serial.print("charge_voltage_after_ignition = ");
    Serial.print(_hvu__ignition_cap_voltage_after);
    Serial.println();    

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

        Serial.print("[ HVU ]");
        Serial.println();
        Serial.print("sm_change_hvu = ");
        Serial.print(_hvu__sm_state_last,HEX);
        Serial.print("->");
        Serial.print(_hvu__sm_state_next,HEX);
        Serial.println();

        _hvu__sm_state = _hvu__sm_state_next;
    }

    switch(_hvu__sm_state) {
        case HVU__SM_STATE_OFF:
            // science power enabled, change state to standby and wait for commands
            _hvu__sm_state_next = HVU__SM_STATE_STANDBY;

            // eneble hvu enable pin to enable charging
            // digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );
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
                // digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );
                digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);
                digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_DISABLE);

                
            }

            if( _hvu__request_state_change == HVU__SM_STATE_CHARGING ) 
            {
                _hvu__sm_state_next = HVU__SM_STATE_CHARGING;

                // enable hvu
                // digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

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
                // digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

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
                // digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

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
                // digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

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
            
            
            Serial.print("[ HVU ]");
            Serial.println();
            Serial.print("aborted");
            Serial.println();
            Serial.print("hvu_abort_error_code = ");
            Serial.print(_hvu__abort_error_code);
            Serial.println();

            _hvu__target_voltage    = 0;
            _hvu__ignition_voltage  = 0;
        break;

        case HVU__SM_STATE_ABORT:
            _hvu__sm_state_next = HVU__SM_STATE_STANDBY;

            // disable all ports regarding charging
            // digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );
            digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);
            digitalWrite( DOUT_HVU_TRIGGER, STATE_HVU_TRIGGER_DISABLE);

            Serial.print("[ HVU ]");
            Serial.println();
            Serial.print("aborted");
            Serial.println();

            _hvu__target_voltage    = 0;
            _hvu__ignition_voltage  = 0;
        break;

        default:
            _hvu__sm_state_next = HVU__SM_STATE_OFF;        
            _hvu__target_voltage    = 0;
            _hvu__ignition_voltage  = 0;
        break;
    }

    
    _hvu__sm_state_last = _hvu__sm_state;

    _hvu__request_state_change = 0x0;
}

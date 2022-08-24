/***********************************************************************
* DESCRIPTION :
*       main arduino code for EXCISS version 4, PCB revision 1.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    18 Feb 2018
*
**/



#include "exciss.h"

BusMaster busMaster;

COM com;

// -------------------------------
// begin: science state machine variables

// state machine variables
uint32_t _sm_science__state_last    = 0x0;
uint32_t _sm_science__state         = 0x0;
uint32_t _sm_science__state_next    = SM_SCIENCE__INIT;

// state machine delay variables
unsigned long    _sm_science__delay_start    = 0UL;
unsigned long    _sm_science__delay_millis   = 0UL;
uint32_t         _sm_science__delay_state_next;

uint8_t _c_boolean_true     = 0xAA;
uint8_t _c_boolean_false    = 0x55;

// state machine volatile constants
uint8_t _c_sm_mcu__state_init           = SM_MCU__STATE_INIT;
uint8_t _c_sm_mcu__state_healthcheck    = SM_MCU__STATE_HEALTHCHECK;
uint8_t _c_sm_mcu__state_normal         = SM_MCU__STATE_NORMAL;
uint8_t _c_sm_mcu__state_ptc_lock       = SM_MCU__STATE_PTC_LOCK;

// state machine
uint8_t _c_sm_science__init                         = SM_SCIENCE__INIT;
uint8_t _c_sm_science__off                          = SM_SCIENCE__OFF;
uint8_t _c_sm_science__scu_bootup_condition_check   = SM_SCIENCE__SCU_BOOTUP_CONDITION_CHECK;
uint8_t _c_sm_science__scu_bootup                   = SM_SCIENCE__SCU_BOOTUP;
uint8_t _c_sm_science__scu_running                  = SM_SCIENCE__SCU_RUNNING;
uint8_t _c_sm_science__scu_shutdown                 = SM_SCIENCE__SCU_SHUTDOWN;
uint8_t _c_sm_science__scu_shutdown_complete        = SM_SCIENCE__SCU_SHUTDOWN_COMPLETE;


// end: science state machine variables
// -------------------------------





unsigned long    _sm_science__scu_keepalive    = SCU__DEFAULT_SHUTDOWN_DELAY    * 1000UL;
unsigned long    _sm_science__scu_wakeup       = SCU__DEFAULT_WAKEUP_INTERVAL   * 1000UL;

void setup() 
{
    
    // init uarts
    com = COM();

    com.begin(19200); // 9600, 19200

    cli();
    wdt_reset();
    /* Start timed  equence */
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    /* Set new prescaler(time-out) value = (~8 s) */
    WDTCSR  = (1<<WDE) | (1<<WDP3) | (1<<WDP0);
    sei();

    initConstants();
}


void initConstants() 
{
    _c_sm_mcu__state_init           = SM_MCU__STATE_INIT;
    _c_sm_mcu__state_healthcheck    = SM_MCU__STATE_HEALTHCHECK;
    _c_sm_mcu__state_normal         = SM_MCU__STATE_NORMAL;
    _c_sm_mcu__state_ptc_lock       = SM_MCU__STATE_PTC_LOCK;

    // science state machine constants
    _c_sm_science__init                         = SM_SCIENCE__INIT;
    _c_sm_science__off                          = SM_SCIENCE__OFF;
    _c_sm_science__scu_bootup_condition_check   = SM_SCIENCE__SCU_BOOTUP_CONDITION_CHECK;
    _c_sm_science__scu_bootup                   = SM_SCIENCE__SCU_BOOTUP;
    _c_sm_science__scu_running                  = SM_SCIENCE__SCU_RUNNING;
    _c_sm_science__scu_shutdown                 = SM_SCIENCE__SCU_SHUTDOWN;
    _c_sm_science__scu_shutdown_complete        = SM_SCIENCE__SCU_SHUTDOWN_COMPLETE;
}


void loop() 
{

    static  uint8_t _sm_mcu__state      = 0x00;

    static  uint8_t _sm_mcu__state_last = 0xFF;

    static  uint8_t _sm_mcu__state_next = _c_sm_mcu__state_init;

    
    if(_sm_mcu__state_last != _sm_mcu__state_next) 
    {
        // state machine state transition detected

        com.log("[ MCU ]");
        com.newline();
        com.log("SM_CHANGE = ");
        com.logHEX(_sm_mcu__state_last);
        com.log("->");
        com.logHEX(_sm_mcu__state_next);
        com.newline();

        _sm_mcu__state      = _sm_mcu__state_next;
        _sm_mcu__state_last = _sm_mcu__state; // update current sm state to detect state transitions
    }

    if(_sm_mcu__state == _c_sm_mcu__state_init) 
    {

        _sm_mcu__state_next = MCU__init();

        com.log("[ MCU ]");
        com.newline();
        com.log("MCU init done");
        com.newline();
        
    }

    if(_sm_mcu__state == _c_sm_mcu__state_healthcheck) 
    {

        com.log("[ MCU ]");
        com.newline();
        com.log("start health check");
        com.newline();

        _sm_mcu__state_next = MCU__healthcheck();

        com.log("[ MCU ]");
        com.newline();
        com.log("done health check");
        com.newline();

    }

    if(_sm_mcu__state == _c_sm_mcu__state_normal) 
    {
        POWER__loop();

        com.loop();

        MCU__loop_science_statemachine();

    }

    if(_sm_mcu__state == _c_sm_mcu__state_ptc_lock) 
    {
        // during the previews operation, a fuse tripping caused a complete power failure.
        // a human intercation is required to resset the fuse trip flag.
        // to resset a power cycle is required.
        // inhibit any science functions.
        POWER__loop();

        com.loop();
    }


    wdt_reset();

}


void MCU__loop_science_statemachine() 
{

    uint8_t bootup_condition = _c_boolean_false;
    

    // state machine delay handling
    if( millis() - _sm_science__delay_start > _sm_science__delay_millis && _sm_science__delay_millis != 0UL )       // TODO: make it robust against CR.Event
    {
        com.log("[ MCU ][ SCIENCE ]");
        com.newline();
        com.log("sm_science_delay = ");
        com.logHEX(_sm_science__delay_state_next);
        com.newline();

        _sm_science__state_next = _sm_science__delay_state_next;

        _sm_science__delay_start        = 0UL;
        _sm_science__delay_millis       = 0UL;
        _sm_science__delay_state_next   = 0x0;
    }
    
    // state machine transitions handling
    if( _sm_science__state_last != _sm_science__state_next ) 
    {

        com.log("[ MCU ][ SCIENCE ]");
        com.newline();
        com.log("sm_change_science = ");
        com.logHEX(_sm_science__state_last);
        com.log("->");
        com.logHEX(_sm_science__state_next);
        com.newline();

        _sm_science__state      = _sm_science__state_next;
        _sm_science__state_last = _sm_science__state; // update current sm state to detect state transitions
    


        if( _sm_science__state == _c_sm_science__init )
        {
            // initial state after system powers up.
            // this state should be executet only once after system power up.
            // set timers to start scu in recovery mode

            // TIMEKEEPER__timer_scu_wakeup( SCU__ENTER_RECOVERYMODE_AFTER_POWERUP );
            POWER__mode_minimal();

            _sm_science__delay_start        = millis();
            _sm_science__delay_millis       = SCU__ENTER_RECOVERYMODE_AFTER_POWERUP * 1000UL;
            _sm_science__delay_state_next   = _c_sm_science__scu_bootup_condition_check;

            com.log("[ MCU ][ SCIENCE ]");
            com.newline();
            com.log("SM_SCIENCE__INIT = ");
            com.log_uint32_t( _sm_science__delay_millis );
            com.newline();
        }

        if( _sm_science__state == _c_sm_science__off )
        {
            // mcu idle until scu required to be activated
            // activation can be commenced due to folowing conditions:
            // a. due to wakeup timer event, scu is requested to do science
            
            POWER__mode_minimal();

            // set next boot-up timer
            _sm_science__delay_start        = millis();
            _sm_science__delay_millis       = _sm_science__scu_wakeup;
            _sm_science__delay_state_next   = _c_sm_science__scu_bootup_condition_check;
        }
                
        
        if( _sm_science__state == _c_sm_science__scu_bootup_condition_check )
        {
            // check start conditions
            // a. power is good
            // b. enough battery power
            // c. not in padded datastranfer window
            // d. if not in tranfser window and if the startcondition was for 6 hours 
            //      not met, start scu any way. 
            //      This gives the ground crew a opportunity to change 
            //      start condition by profile upload.
            // 
            // if the condition doesn't meets the boot-up requirements,
            // change to SM_SCIENCE__OFF state and resset scu wakup timer
            // to default wakup timer value (5min).

            com.log("[ MCU ][ SCIENCE ]");
            com.newline();
            

            bootup_condition = _c_boolean_true;

            // a. is usb power good?
            if( POWER__charg_ok() == _c_boolean_false ) // 0xAA = good, 0x55 = bad
            {
                // power is not good don't start the scu
                bootup_condition = _c_boolean_false;

                com.log("POWER__charg_ok = 0");
                com.newline();
            }


            // b. enough battery power
            // NOTE: battery power status check disabled. start scu any way.
            // POWER_battery_enough_power_for_scu_start() returns only 0xAA
            // 
            if( POWER_battery_enough_power_for_scu_start() == _c_boolean_false ) 
            {
                // due to low battery level inhibit scu bootup.
                // battery level start condition is defined in config_power.h
                bootup_condition = _c_boolean_false;

                _sm_science__scu_wakeup       = 60000UL; // next try in 2min

                com.log("POWER_battery_enough_power_for_scu_start = 0");
                com.newline();
            }

            // c. not in padded datastranfer window
            if( TIMEKEEPER__ok_to_bootup_scu() == _c_boolean_false )
            {
                // is in transfer window or not enough time til transfer window. don't start scu.
                bootup_condition = _c_boolean_false;

                _sm_science__scu_wakeup       = 60000UL; // next try in 2min

                com.log("TIMEKEEPER__ok_to_bootup_scu = 0");
                com.newline();
            }


            

            if( bootup_condition == _c_boolean_true )
            {
                // boot-up condition is ok, start scu boot-up procedures
                _sm_science__state_next = _c_sm_science__scu_bootup;

                com.log("scu_start_condition_check = 1");
                com.newline();
            }
            else
            {
                // boot-up condition is ok, start scu boot-up procedures
                _sm_science__state_next = _c_sm_science__off;

                com.log("scu_start_condition_check = 0");
                com.newline();
            }
        }
         

        if( _sm_science__state == _c_sm_science__scu_bootup )
        {
            // required power path for bootup:
            // a. usb hub
            // b. usb-x
            // c. scu power
            // other task before bootup:
            // switch usb-x to scu
            // set default shutdown timer

            SCU__request_bootup();

            // set default keep alive timer
            _sm_science__delay_start      = millis();
            _sm_science__delay_millis     = SCU__DEFAULT_SHUTDOWN_DELAY * 1000UL;
            _sm_science__delay_state_next = _c_sm_science__scu_shutdown;

            // set default wakup delay
            _sm_science__scu_wakeup = SCU__DEFAULT_WAKEUP_INTERVAL * 1000UL;

            // enter scu running state
            _sm_science__state_next = _c_sm_science__scu_running;        
        }
        
        if( _sm_science__state == _c_sm_science__scu_running )
        {
            // wait for:
            // a. shutdown request due to frame power cut off detected via power-good
            //      this will be triggerd by POWER.cpp
            // b. shutdown request triggered by the state machine delay
            // c. maximumg runtime exceeded
            // d. battery too low

        }

        if( _sm_science__state == _c_sm_science__scu_shutdown )
        {
            // a. remove science power imediently
            // b. keep usb massstorage power until shutdown complete state
            // c. start timeout until shutdown complete state
            // d. if scu was in "recovery" mode change mode to "science"
            // e. wait given time until execute shutdown complete precedures

            SCU__request_shutdown();

            // set delay for shutdown complete procedure
            _sm_science__delay_start        = millis();
            _sm_science__delay_millis       = SCU__DELAY_BOOTDOWN_FINALISE * 1000UL;
            _sm_science__delay_state_next   = _c_sm_science__scu_shutdown_complete;
        }
                
        
        if( _sm_science__state == _c_sm_science__scu_shutdown_complete )
        {
            // a. switch usb-x to iss
            // b. usb-x power power on
            // c. remove scu power
            // d. remove usb-hub, usb-a, usb-b power
            // e. check if scu requested a "rom" command for scu functionality check.
            //      if scu doesn't requested a "rom", it assumed the mmc have some issue
            //      at the next scu boot-up the spare mmc is used.


            SCU__bootdown_finalise();

            POWER__mode_minimal();

            _sm_science__state_next = _c_sm_science__off;
        }        

    }

    if( _sm_science__state != _c_sm_science__off && _sm_science__state != _c_sm_science__init ) 
    {
        SCU__loop();

        HVU__loop();

        VIBRATION_MOTOR__loop();
    }
    
}


void MCU__set_science_scu_keep_alive(unsigned long aSeconds)
{
    _sm_science__scu_keepalive = aSeconds * 1000UL;

    if(_sm_science__state == _c_sm_science__scu_running) 
    {
        _sm_science__delay_start        = millis();
        _sm_science__delay_millis       = _sm_science__scu_keepalive;
        _sm_science__delay_state_next   = _c_sm_science__scu_shutdown;
    }
}

void MCU__set_science_scu_wakup(unsigned long aSeconds)
{
    _sm_science__scu_wakeup = aSeconds * 1000UL;

    if(
        _sm_science__state == _c_sm_science__off 
        || _sm_science__state == _c_sm_science__init ) 
    {
        _sm_science__delay_start        = millis();
        _sm_science__delay_millis       = _sm_science__scu_wakeup;
        _sm_science__delay_state_next   = _c_sm_science__scu_bootup_condition_check;
    }
}


uint32_t MCU__init() 
{
    
    busMaster.initBus();
    // wdt_reset(); // reset watchdog timer

    POWER__init();

    TIMEKEEPER__init();

    USBSTORAGE__init();

    uint8_t power_mode = POWER__mode();

    if(power_mode == 0xAA ) 
    {
        HVU__init();

        VIBRATION_MOTOR__init();

        SCU__init();

        LIGHTS__init();

        TEMPSENS__init();

        POWER__mode_minimal();    

        return _c_sm_mcu__state_healthcheck;
    }
    else 
    {
        return _c_sm_mcu__state_ptc_lock;
    }
    
    wdt_enable(WDTO_8S); // set watchdog interval to 8 seconds
}


uint32_t MCU__healthcheck() {

    TIMEKEEPER__healthcheck();

    return _c_sm_mcu__state_normal;
}


void MCU__log_timer_status()
{
    unsigned long sec = 0UL;
    com.log("[ MUC ][ SM ]");
    com.newline();

    com.log("[ keep alive ]");
    com.newline();
    com.log("keep_alive = ");
    sec = _sm_science__scu_keepalive / 1000UL;
    com.log_uint32_t( sec );
    com.newline();
    com.log("keep_alive_remaining_sec = ");
    if( _sm_science__delay_state_next == _c_sm_science__scu_shutdown )
    {
        sec = ( _sm_science__delay_millis - ( millis() - _sm_science__delay_start ) ) / 1000UL;
        com.log_uint32_t(sec);
    } else {
        com.log("not active");
    }
    com.newline();


    com.log("[ wakeup ]");
    com.newline();
    com.log("wakeup = ");
    sec = _sm_science__scu_wakeup / 1000UL;
    com.log_uint32_t( sec );
    com.newline();
    com.log("wakeup_remaining_sec = ");
    if( _sm_science__delay_state_next == _c_sm_science__scu_bootup_condition_check )
    {
        sec = ( _sm_science__delay_millis - ( millis() - _sm_science__delay_start ) ) / 1000UL;
        com.log_uint32_t(sec);
    } else {
        com.log("not active");
    }
    com.newline();
}
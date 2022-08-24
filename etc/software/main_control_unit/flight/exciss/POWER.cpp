/***********************************************************************
*
* DESCRIPTION :
* AUTHOR :    Shintaro Fujita
*
**/

#include "config_power.h" 
#include "POWER.h" 
#include "exciss.h" 
#include "PSU.h" 
#include <EEPROM.h>


unsigned long POWER__power_down_time = 0;

uint8_t _power__power_mode;

uint8_t _power__science_status = 0x55;

uint8_t _power__current_battery_level = 0; // battery level value is in percentage

void POWER__init() {
  // init main power path switches
  pinMode( DOUT_VSYS_ENABLE, OUTPUT );
  digitalWrite( DOUT_VSYS_ENABLE , STATE_VSYS_OFF );

  pinMode( DOUT_12V_ENABLE , OUTPUT );
  digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );

  pinMode( DOUT_SCU_PWR_ENABLE , OUTPUT );
  digitalWrite( DOUT_SCU_PWR_ENABLE , STATE_SCU_PWR_OFF );


  // init peripherial power switches

  // usb mass storage power switches
  pinMode( DOUT_USBHUB_PWR , OUTPUT );
  digitalWrite( DOUT_USBHUB_PWR , STATE_USBHUB_PWR_OFF );

  pinMode( DOUT_USB_X_PWR , OUTPUT );
  digitalWrite( DOUT_USB_X_PWR , STATE_USB_OFF );

  pinMode( DOUT_USB_A_PWR , OUTPUT );
  digitalWrite( DOUT_USB_A_PWR , STATE_USB_OFF );

  pinMode( DOUT_USB_B_PWR , OUTPUT );
  digitalWrite( DOUT_USB_B_PWR , STATE_USB_OFF );


  // scu power 
  pinMode( DOUT_SCU_PWR_ENABLE , OUTPUT );
  digitalWrite( DOUT_SCU_PWR_ENABLE , STATE_SCU_PWR_OFF );


  // vibration motor driver
  pinMode( DOUT_3V_ENABLE , OUTPUT );
  digitalWrite( DOUT_3V_ENABLE , STATE_3V_OFF );

  pinMode( DOUT_MOT_ENABLE, OUTPUT );
  digitalWrite( DOUT_MOT_ENABLE , STATE_MOT_DISABLE );
  
  
  PSU__init();


  // ----------------------------
  // check PSU F6000 fuse tripped
  uint8_t psu_ptc_tripped_yes_counter   = 0;
  uint8_t psu_ptc_tripped_no_counter    = 0;

  uint8_t psu_ptc_tripped_a = NONVOLATILEMEM__read(1);
  uint8_t psu_ptc_tripped_b = NONVOLATILEMEM__read(2);
  uint8_t psu_ptc_tripped_c = NONVOLATILEMEM__read(3);
  uint8_t psu_ptc_tripped_d = NONVOLATILEMEM__read(4);
  uint8_t psu_ptc_tripped_e = NONVOLATILEMEM__read(5);

  com.log("[ POWER ]");
  com.newline();
  com.log("psu_ptc_tripped_a = ");
  com.log(psu_ptc_tripped_a);
  com.newline();
  com.log("psu_ptc_tripped_b = ");
  com.log(psu_ptc_tripped_b);
  com.newline();
  com.log("psu_ptc_tripped_c = ");
  com.log(psu_ptc_tripped_c);
  com.newline();
  com.log("psu_ptc_tripped_d = ");
  com.log(psu_ptc_tripped_d);
  com.newline();
  com.log("psu_ptc_tripped_e = ");
  com.log(psu_ptc_tripped_e);
  com.newline();


  psu_ptc_tripped_a == 0xAA ? psu_ptc_tripped_yes_counter++ : false;
  psu_ptc_tripped_b == 0xAA ? psu_ptc_tripped_yes_counter++ : false;
  psu_ptc_tripped_c == 0xAA ? psu_ptc_tripped_yes_counter++ : false;
  psu_ptc_tripped_d == 0xAA ? psu_ptc_tripped_yes_counter++ : false;
  psu_ptc_tripped_e == 0xAA ? psu_ptc_tripped_yes_counter++ : false;


  psu_ptc_tripped_a == 0x55 ? psu_ptc_tripped_no_counter++ : false;
  psu_ptc_tripped_b == 0x55 ? psu_ptc_tripped_no_counter++ : false;
  psu_ptc_tripped_c == 0x55 ? psu_ptc_tripped_no_counter++ : false;
  psu_ptc_tripped_d == 0x55 ? psu_ptc_tripped_no_counter++ : false;
  psu_ptc_tripped_e == 0x55 ? psu_ptc_tripped_no_counter++ : false;


  PSU__mode_mcu_only(); // input limit 200mAh, charg current 0mAh

  if(psu_ptc_tripped_no_counter > psu_ptc_tripped_yes_counter) 
  {
    _power__power_mode = 0xAA; // normal operation mode

    com.log("[ POWER ]");
    com.newline();
    com.log("power_mode = normal");
    com.newline();
  }

  if(psu_ptc_tripped_yes_counter > psu_ptc_tripped_no_counter) 
  {
    _power__power_mode = 0x55; // keep vsys disabled until power cycle

    // set psu to minimal current mode
    PSU__mode_mcu_only(); // input limit 200mAh, charg current 0mAh

    com.log("[ POWER ]");
    com.newline();
    com.log("power_mode = F6000 fuse tripped");
    com.newline();
  }

  // preset the fuse trip indicator flag to F6000 fuse tripped in eeprom.
  // if the fuse tripped this flag remains in eeprom, and can be used as 
  // a uncontrolled power loss due to fuser tripping or other powre path issue.
  NONVOLATILEMEM__write(1,0xAA);
  NONVOLATILEMEM__write(2,0xAA);
  NONVOLATILEMEM__write(3,0xAA);
  NONVOLATILEMEM__write(4,0xAA);
  NONVOLATILEMEM__write(5,0xAA);

  // end: check PSU F6000 fuse tripped
  // ----------------------------

}

void POWER__loop() 
{
  POWER__loop_check_charg_ok();

  // POWER__loop_battery_level_check();
}

uint8_t POWER__charg_ok() 
{
  // return power good status.
  // power is not good if the USB power is down.
  return PSU__charg_ok();
}

uint8_t POWER_battery_enough_power_for_scu_start()
{
  static uint8_t ret_enough_power = 0x55;

  ret_enough_power = 0x55;

  // uint8_t battery_level = PSU__battery_level();

  if( _power__current_battery_level > POWER__MIN_BATTERY_LEVEL_SCU_START )
  {
    ret_enough_power = 0xAA;
  }


  // NOTE: battery power status check disabled. start scu any way.
  // The SCU 
  ret_enough_power = 0xAA;

  return ret_enough_power;
}


uint8_t POWER_battery_enough_power_for_scu_run()
{
  static uint8_t ret_enough_power = 0x55;
  
  ret_enough_power = 0x55;

  // uint8_t battery_level = PSU__battery_level();

  if( _power__current_battery_level > POWER__MIN_BATTERY_LEVEL_SCU_RUN )
  {
    ret_enough_power = 0xAA;
  }

  return ret_enough_power; 
}


uint8_t POWER__mode() 
{
  // return current power mode
  return _power__power_mode;
}


void POWER__loop_battery_level_check()
{
  static unsigned long timeout_start = 0UL;
  static unsigned long timeout_delay = 0UL; // check every 10 seconds

  if( millis() - timeout_start >= timeout_delay )
  {
    timeout_start = millis();
    timeout_delay = 10000UL;

    _power__current_battery_level = PSU__battery_level();

  }
}

void POWER__loop_check_charg_ok() 
{
  static uint8_t previews_charg_ok = 0;
  static unsigned long last_power_down_seconds = 0UL;

  // interval timer for charger setting update
  static unsigned long next_charger_update_timer_start = 0UL;
  static unsigned long next_charger_update_timer_delay = 10000UL;

  // timeout for deglitch the usb power offline
  static unsigned long timeout_deglitch_charg_not_ok_start = 0UL;
  static unsigned long timeout_deglitch_charg_not_ok_delay = 3000UL;
  static uint8_t powerdown_deglitch = 0x55;

  uint8_t charg_ok = PSU__charg_ok();

  // ----------------------------------------------------------------------------
  // transition from power good to power down
  if(charg_ok != previews_charg_ok && charg_ok == 0x55) 
  {
    _power__power_mode == 0xAA ? PSU__mode_normal() : PSU__mode_mcu_only(); // input limit 850mAh, charg current 350mAh
    
    // set PSU F6000 fuse tripped indicator 
    NONVOLATILEMEM__write(1,0x55); // normal shutdown value
    NONVOLATILEMEM__write(2,0x55); // normal shutdown value
    NONVOLATILEMEM__write(3,0x55); // normal shutdown value
    NONVOLATILEMEM__write(4,0x55); // normal shutdown value
    NONVOLATILEMEM__write(5,0x55); // normal shutdown value

    powerdown_deglitch = 0xAA;

    // wait 1 sec before execute shutdown procedures
    timeout_deglitch_charg_not_ok_start = millis();
    timeout_deglitch_charg_not_ok_delay = 2000UL;

    // set power donw counter
    POWER__power_down_time = millis();
    last_power_down_seconds = 0;
  }


  // timer for deglitching powerdown events.
  if( powerdown_deglitch == 0xAA && 
      ((millis() - timeout_deglitch_charg_not_ok_start) > timeout_deglitch_charg_not_ok_delay) )
  {
    // usb power was not recovered in 1 sec, procceed with shutdown procedures
    powerdown_deglitch = 0x55;

    com.log("[ POWER ]");
    com.newline();
    com.log("power_good = false");
    com.newline();
    com.log("request shutdown");
    com.newline();

    _power__power_mode == 0xAA ? PSU__mode_normal() : PSU__mode_mcu_only(); // input limit 850mAh, charg current 350mAh

    // request scu shutdwon
    SCU__request_shutdown();
  }


  // ----------------------------------------------------------------------------
  // transition from power down to power good
  if(charg_ok != previews_charg_ok && charg_ok == 0xAA) 
  {

    _power__power_mode == 0xAA ? PSU__mode_normal() : PSU__mode_mcu_only();
    
    powerdown_deglitch = 0x55;

    // set PSU F6000 fuse tripped indicator 
    NONVOLATILEMEM__write(1,0xAA); // if this value is present after a power cycle, the fuse tripped
    NONVOLATILEMEM__write(2,0xAA); // if this value is present after a power cycle, the fuse tripped
    NONVOLATILEMEM__write(3,0xAA); // if this value is present after a power cycle, the fuse tripped
    NONVOLATILEMEM__write(4,0xAA); // if this value is present after a power cycle, the fuse tripped
    NONVOLATILEMEM__write(5,0xAA); // if this value is present after a power cycle, the fuse tripped

    com.log("[ POWER ]");
    com.newline();
    com.log("power_good = true");
    com.newline();
  }

  // ----------------------------------------------------------------------------
  // charg not ok counter
  if(charg_ok == 0x55) 
  {
    // if charg not ok, start the count down and plot to the serial port.
    // it's mainly usefull for debugging.
    unsigned long power_down_seconds = ( (millis() - POWER__power_down_time) / 1000UL );

    if( power_down_seconds != last_power_down_seconds ) {
      com.log("sec_since_powerdown = ");
      com.log((int)power_down_seconds);
      com.newline();
      last_power_down_seconds = power_down_seconds;
    }

  }

  // ----------------------------------------------------------------------------
  // update charg value 

  if(charg_ok == 0xAA && (millis() - next_charger_update_timer_start > next_charger_update_timer_delay) )  
  {
    // the charg must be periodical updated with the desired charge current.
    next_charger_update_timer_start = millis();
    next_charger_update_timer_delay = 1000UL * 10UL;

    if(_power__power_mode == 0xAA ) 
    {
      PSU__mode_normal(); // input limit 850mAh, charg current 350mAh
    }
  }

  
  previews_charg_ok = charg_ok;
}


void POWER__mode_minimal() {

  digitalWrite( DOUT_USBHUB_PWR , STATE_USBHUB_PWR_OFF );

  digitalWrite( DOUT_USB_X_PWR , STATE_USB_ON );

  digitalWrite( DOUT_USB_A_PWR , STATE_USB_OFF );

  digitalWrite( DOUT_USB_B_PWR , STATE_USB_OFF );

  digitalWrite( DOUT_VSYS_ENABLE , STATE_VSYS_OFF );

  digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );

  digitalWrite( DOUT_MOT_ENABLE , STATE_MOT_DISABLE );

  digitalWrite( DOUT_3V_ENABLE , STATE_3V_OFF );

  digitalWrite( DOUT_SCU_PWR_ENABLE , STATE_SCU_PWR_OFF );

}

void POWER__mode_scu_bootup_prepare() {
  com.log("[ POWER ]");
  com.newline();
  com.log("mode sci boot prepare");
  com.newline();

  // delay(1000);
  
  // power changes before scu powering up
  digitalWrite( DOUT_3V_ENABLE , STATE_3V_OFF );

  digitalWrite( DOUT_USBHUB_PWR , STATE_USBHUB_PWR_ON );

  digitalWrite( DOUT_USB_X_PWR , STATE_USB_ON );

  digitalWrite( DOUT_USB_A_PWR , STATE_USB_OFF );

  digitalWrite( DOUT_USB_B_PWR , STATE_USB_OFF );
}

void POWER__mode_scu_bootup() {
  // note: before powering the scu the periferial should be powerd first
  com.log("[ POWER ]");
  com.newline();
  com.log("mode sci bootup");
  com.newline();

  digitalWrite( DOUT_SCU_PWR_ENABLE , STATE_SCU_PWR_ON );
}


// --------------------------
// science power mode methods

uint8_t POWER__science_power_enabled() {
  return _power__science_status;
}

void POWER__mode_start_science_experiment() {
  com.log("[ POWER ]");
  com.newline();
  com.log("mode sci power on");
  com.newline();

  _power__science_status = 0xAA;

  digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);

  digitalWrite( DOUT_VSYS_ENABLE , STATE_VSYS_ON );

  // digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );

  digitalWrite( DOUT_3V_ENABLE, STATE_3V_ON );

  digitalWrite( DOUT_MOT_ENABLE , STATE_MOT_ENABLE );

}

void POWER__mode_end_science_experiment() {
  com.log("[ POWER ]");
  com.newline();
  com.log("mode sci power off");
  com.newline();

  _power__science_status = 0x55;

  digitalWrite( DOUT_HVU_ENABLE, STATE_HVU_DISABLE);

  digitalWrite( DOUT_VSYS_ENABLE , STATE_VSYS_OFF );

  digitalWrite( DOUT_12V_ENABLE , STATE_12V_OFF );

  digitalWrite( DOUT_3V_ENABLE, STATE_3V_OFF);

  digitalWrite( DOUT_MOT_ENABLE , STATE_MOT_DISABLE );


}

void POWER__enable_vibration_motor() 
{
  digitalWrite( DOUT_3V_ENABLE , STATE_3V_ON );

  digitalWrite( DOUT_MOT_ENABLE , STATE_MOT_ENABLE );
}

void POWER__disable_vibration_motor() 
{
  digitalWrite( DOUT_3V_ENABLE , STATE_3V_OFF );

  digitalWrite( DOUT_MOT_ENABLE , STATE_MOT_DISABLE );
}


void POWER__logout_charger() 
{
  PSU__logoutCharger();
}

void POWER__logout_gauge() 
{
  PSU__logoutGauge(); 
}

void POWER__logout_power_switches() 
{
  com.log("[ POWER ]");
  com.newline();
  com.log("vsys = ");
  digitalRead(DOUT_VSYS_ENABLE) == STATE_VSYS_ON ? com.log("on") : com.log("off");
  com.newline();

  com.log("3V = ");
  digitalRead(DOUT_3V_ENABLE) == STATE_3V_ON ? com.log("on") : com.log("off");
  com.newline();

  com.log("12V = ");
  digitalRead(DOUT_12V_ENABLE) == STATE_12V_ON ? com.log("on") : com.log("off");
  com.newline();

  com.log("SCU = ");
  digitalRead(DOUT_SCU_PWR_ENABLE) == STATE_SCU_PWR_ON ? com.log("on") : com.log("off");
  com.newline();

  com.log("USB_X = ");
  digitalRead(DOUT_USB_X_PWR) == STATE_USB_ON ? com.log("on") : com.log("off");
  com.newline();

  com.log("USB_A = ");
  digitalRead(DOUT_USB_A_PWR) == STATE_USB_ON ? com.log("on") : com.log("off");
  com.newline();

  com.log("USB_B = ");
  digitalRead(DOUT_USB_B_PWR) == STATE_USB_ON ? com.log("on") : com.log("off");
  com.newline();

  com.log("USB_HUB = ");
  digitalRead(DOUT_USBHUB_PWR) == STATE_USBHUB_PWR_ON ? com.log("on") : com.log("off");
  com.newline();  
}

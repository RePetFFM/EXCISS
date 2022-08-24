/***********************************************************************
* DESCRIPTION :
*
* 
* AUTHOR :    Shintaro Fujita
*
**/



#include "VIBRATION_MOTOR.h"
#include "config_VIBRATION_MOTOR.h"
#include "DRV2605_I2C.h"
#include "exciss.h"
// uint8_t _effects[8];

DRV2605 drv;

uint8_t _vibration_motor__go = 0x55;

// unsigned long _vibration_motor__max_run_timeout = 0UL;

unsigned long _vibration_motor__max_run_start = 0UL;
unsigned long _vibration_motor__max_run_delay = 0UL;


// status
uint8_t _vibration_motor__reg_over_current_detect   = 0;

uint8_t _vibration_motor__reg_over_temp_detect      = 0;

uint8_t _vibration_motor__reg_device_id_detect      = 0;

uint8_t _vibration_motor__driver_available          = 0xAA;


void VIBRATION_MOTOR__init() 
{
  
}


uint8_t VIBRATION_MOTOR__read_status_register()
{
  uint8_t vm_reg = busMaster.readByte(0x5A, 0x00);

  return vm_reg;
} 

void VIBRATION_MOTOR__check_driver_status() 
{
  uint8_t vm_reg = VIBRATION_MOTOR__read_status_register();

  (vm_reg & 0x01) == 0x01 ? _vibration_motor__reg_over_current_detect   = 1 : _vibration_motor__reg_over_current_detect = 0;

  (vm_reg & 0x02) == 0x02 ? _vibration_motor__reg_over_temp_detect      = 1 : _vibration_motor__reg_over_temp_detect = 0;

  (vm_reg & 0xE0) == 0xE0 ? _vibration_motor__reg_device_id_detect      = 1 : _vibration_motor__reg_device_id_detect = 0;

  _vibration_motor__reg_device_id_detect = 1 ? _vibration_motor__driver_available = 0xAA : _vibration_motor__driver_available = 0x55;
}


void VIBRATION_MOTOR__logout_driver_status() 
{
  VIBRATION_MOTOR__check_driver_status();

  uint8_t vm_reg = VIBRATION_MOTOR__read_status_register();

  com.log("[ VM ]");
  com.newline();

  com.log("vm_status_register = ");
  com.logBIN(vm_reg);
  com.newline();

  com.log("vm_reg_over_current_detect = ");
  com.log(_vibration_motor__reg_over_current_detect);
  com.newline();

  com.log("vm_reg_over_temp_detect = ");
  com.log(_vibration_motor__reg_over_temp_detect);
  com.newline();

  com.log("vm_reg_device_id_detect = ");
  com.log(_vibration_motor__reg_device_id_detect);
  com.newline();

  com.newline();
}

void VIBRATION_MOTOR__init_driver() 
{

  // check motor driver status and plot

  // VIBRATION_MOTOR__check_driver_status();

  POWER__enable_vibration_motor();

  delay(1); // 100

  VIBRATION_MOTOR__logout_driver_status();

  delay(1); // 100

  if( _vibration_motor__driver_available == 0x55) {
    // driver not responding

    com.log("driver not responding");
    com.newline();

    // VIBRATION_MOTOR__logout_driver_status();

    // return;
  }

  

  // busMaster.switchMuxPortByDeviceAddress(0x18);
  
  // enable required power
  

  // switch i2c switch 
  
  

  // initialize haptirc driver
  drv.begin();

  delay(1); // 100
  
  // controled by i2c commands
  drv.setMode(DRV2605_MODE_INTTRIG); 

  // vibration motor is rotating type
  drv.useERM();

  // load 3V library
  drv.selectLibrary(2);

  // init with default value
  // 58_16_16_16_58_16_16_16
  drv.setWaveform(0, 88);
  drv.setWaveform(1, 16);
  drv.setWaveform(2, 16);
  drv.setWaveform(3, 16);
  drv.setWaveform(4, 88);
  drv.setWaveform(5, 16);
  drv.setWaveform(6, 16);
  drv.setWaveform(7, 16);    
}


void VIBRATION_MOTOR__set_waveform(uint8_t form_1, uint8_t form_2, uint8_t form_3, uint8_t form_4, uint8_t form_5, uint8_t form_6, uint8_t form_7, uint8_t form_8 ) {


  if(_vibration_motor__driver_available == 0x55) {
    // return;
  }

  
  drv.setWaveform(0, form_1);
  drv.setWaveform(1, form_2);
  drv.setWaveform(2, form_3);
  drv.setWaveform(3, form_4);
  drv.setWaveform(4, form_5);  
  drv.setWaveform(5, form_6);
  drv.setWaveform(6, form_7);
  drv.setWaveform(7, form_8);
}


/*
 * Set a waveform configuration of exactly 8 values of 0-116.
 * Example "0A_64_0B_64_64_14_14_0". 0 means stop.
 * The waveform is looped by run_Drv().
 */
bool VIBRATION_MOTOR__setup (char *new_Config) 
{
  VIBRATION_MOTOR__init_driver();

  if(_vibration_motor__driver_available == 0x55) 
  {
    // return false;    
  }

  int drv_Config[] = {0,0,0,0,0,0,0,0};
  bool my_Config_OK = true;
  
  char *token =strtok(new_Config,"_");
  int i = 0;
  
  while (token !=NULL)
  {
    int value = strtol(token, NULL, 16);
    if (i<8 && value >=0 && value <= 116) {
      drv_Config[i]=value;
    }
    else {
      my_Config_OK = false;
    }

    token =strtok(NULL,"_");
    i++;
  }

  //Set Config
  if (my_Config_OK) {
    com.log("[ VM ]");
    com.newline();
    com.log("setup = ok");
    com.newline();
    VIBRATION_MOTOR__set_waveform(drv_Config [0],drv_Config [1],drv_Config [2],drv_Config [3],drv_Config [4],drv_Config [5],drv_Config [6], drv_Config [7]);
    return 1;
  } else return 0;
}


void VIBRATION_MOTOR__go() 
{
  if(_vibration_motor__driver_available == 0x55) {
    // return;
  }

  

  // enable power path and set enable pin
  POWER__enable_vibration_motor();

  drv.go();

  // set max run timeout. prevent running the vm forever.
  // _vibration_motor__max_run_timeout = millis() + ( VIBRATION_MOTOR__MAX_RUN_TIMEOUT_SEC * 1000UL );

  _vibration_motor__max_run_start = millis();
  _vibration_motor__max_run_delay = VIBRATION_MOTOR__MAX_RUN_TIMEOUT_SEC * 1000UL;


  // set motor go flag
  _vibration_motor__go = 0xAA;

  com.log("[ VM ]");
  com.newline();
  com.log("start = 1");
  com.newline();
}

void VIBRATION_MOTOR__stop() {

  if(_vibration_motor__driver_available == 0x55) {
    // return;
  }

  

  // resset motor go flag
  _vibration_motor__go = 0x55;

  drv.stop();

  com.log("[ VM ]");
  com.newline();
  com.log("stop = 1");
  com.newline();
}


void VIBRATION_MOTOR__test() {
  if(_vibration_motor__driver_available == 0x55) {
    // return;
  }

    com.log("[ VM ]");
    com.newline();
    com.log("test = 1");
    com.newline();

    POWER__enable_vibration_motor();

    

    delay(500);
    delay(1);

    drv.begin();
    
    delay(100);
    delay(1);

    // I2C trigger by sending 'go' command 
    // default, internal trigger when sending GO command
    drv.setMode(DRV2605_MODE_INTTRIG); 
    drv.useERM();
    drv.selectLibrary(2);

    drv.setWaveform(0, 88);
    drv.setWaveform(1, 16);
    drv.setWaveform(2, 16);
    drv.setWaveform(3, 16);
    drv.setWaveform(4, 88);
    drv.setWaveform(5, 16);
    drv.setWaveform(6, 16);
    drv.setWaveform(7, 16);

    com.log("go soon");
    com.newline();
    delay(1000);
    delay(1);
    drv.go();
}


void VIBRATION_MOTOR__loop() 
{
  if(_vibration_motor__driver_available == 0x55)
  {
    // return;
  }

  // if( _vibration_motor__go == 0xAA && _vibration_motor__max_run_timeout < millis() ) 
  if( _vibration_motor__go == 0xAA && ( (millis() - _vibration_motor__max_run_start) > _vibration_motor__max_run_delay) ) 
  {
    // max run timeout reached, stop vm
    _vibration_motor__go = 0x55;

    drv.stop();

    com.log("[ VM ]");
    com.newline();
    com.log("stop = 1");
    com.newline();
    com.log("vm_max_run_time = 1");
    com.newline();
  }

  if( _vibration_motor__go == 0xAA )
  {
    
    uint8_t vm_reg = busMaster.readWordLSB_no_interrupt_no_lockup(0x5A, 0x0C);

    vm_reg = vm_reg & 0x01;

    if( vm_reg==0x00 ) 
    {
      drv.go_noI_noL();
    }
  }
}
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

// end: science state machine variables
// -------------------------------

unsigned long    _sm_science__scu_keepalive    = SCU__DEFAULT_SHUTDOWN_DELAY    * 1000UL;
unsigned long    _sm_science__scu_wakeup       = SCU__DEFAULT_WAKEUP_INTERVAL   * 1000UL;

void setup() 
{
    
    // init uarts
    com = COM();

    com.begin(19200); // 9600, 19200


    busMaster.initBus();
    // wdt_reset(); // reset watchdog timer

    
    VIBRATION_MOTOR__init();



    Serial.println("DRV test");


  pinMode( DOUT_VSYS_ENABLE, OUTPUT );
  digitalWrite( DOUT_VSYS_ENABLE , STATE_VSYS_ON );

  pinMode( DOUT_3V_ENABLE , OUTPUT );
  digitalWrite( DOUT_3V_ENABLE , STATE_3V_ON );

  pinMode( PWMOUT_FRONTLIGHT_CONTROL , OUTPUT);
  analogWrite( PWMOUT_FRONTLIGHT_CONTROL , 1);

  pinMode( PWMOUT_BACKLIGHT_CONTROL , OUTPUT);
  analogWrite( PWMOUT_BACKLIGHT_CONTROL , 1);

  pinMode( DOUT_MOT_ENABLE , OUTPUT );
  digitalWrite( DOUT_MOT_ENABLE , HIGH );
  

  // delay(500); // wait for console opening

  uint8_t  scl = digitalRead(21);
  uint8_t  sda = digitalRead(20);

  Serial.println(scl);
  Serial.println(sda);

  busMaster = BusMaster();
  busMaster.initBus();


  

  
  Serial.println("test1");

  // busMaster.switchMuxPortByDeviceAddress(0x5A);

  // busMaster.switchMuxPortByDeviceAddress(0x18);

  Serial.println("test2");
  uint8_t vm_reg = busMaster.readWordLSB(0x5A, 0x00);

  Serial.println(vm_reg,BIN);

  Serial.println("test3");

  VIBRATION_MOTOR__init_driver();
  // delay(1000);

  /*
  VIBRATION_MOTOR__check_driver_status();


  Serial.print("_vibration_motor__driver_available = ");

  Serial.println(_vibration_motor__driver_available,HEX);


  Serial.print("_vibration_motor__reg_device_id_detect = ");

  Serial.println(_vibration_motor__reg_device_id_detect,HEX);  



  bool b = busMaster.switchMuxPortByDeviceAddress(0x5A); // DRV2605



  // scan();

  
  delay(500); // wait for console opening

  Serial.println("DRV go");


  drv.begin();
  drv.setMode(DRV2605_MODE_INTTRIG); 
  drv.useERM();
  // drv.useLRA();
  drv.selectLibrary(2);
  
  // I2C trigger by sending 'go' command 
  // default, internal trigger when sending GO command
  

  drv.setWaveform(0, 88);
  drv.setWaveform(1, 16);
  drv.setWaveform(2, 16);
  drv.setWaveform(3, 16);
  drv.setWaveform(4, 88);
  drv.setWaveform(5, 16);
  drv.setWaveform(6, 16);
  drv.setWaveform(7, 166);

  uint8_t tmpByte;

  tmpByte = drv.readRegister8(DRV2605_REG_STATUS);
  Serial.println(tmpByte,BIN);

  tmpByte = drv.readRegister8(0x03);
  Serial.println(tmpByte,BIN);

  tmpByte = drv.readRegister8(0x21);
  Serial.println(tmpByte);

  tmpByte = drv.readRegister8(0x1D);
  Serial.println(tmpByte,BIN);

  tmpByte = drv.readRegister8(0x1A);
  Serial.println(tmpByte,BIN);
  */

  VIBRATION_MOTOR__go();
}


void loop() 
{



}


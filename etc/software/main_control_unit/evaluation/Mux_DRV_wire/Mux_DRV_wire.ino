/***********************************************************************
* FILENAME :        Mux_RTC.cpp      DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* This version uses the Arduino Wire.h to compare it to the version without Wire.h.
*
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Rainer Haseitl
*
**/

#include "atmega_port_mapping.h"

#include "BusMaster.h"

// #include "DRV2605.h"

#include "Adafruit_DRV2605.h"

BusMaster busMaster;

Adafruit_DRV2605 drv;


// status
uint8_t _vibration_motor__reg_over_current_detect   = 0;

uint8_t _vibration_motor__reg_over_temp_detect      = 0;

uint8_t _vibration_motor__reg_device_id_detect      = 0;

uint8_t _vibration_motor__driver_available          = 0x55;

void setup() 
{
  // initilize BusMaster

  // TCCR5B = TCCR5B & 0b11111000 | 0x01; // 0x01 = 31250Hz/1, 0x02 = 31250Hz/8

  Serial.begin(19200);

  // delay(1000); // wait for console opening

  Serial.println("DRV test");


  pinMode( DOUT_VSYS_ENABLE, OUTPUT );
  digitalWrite( DOUT_VSYS_ENABLE , STATE_VSYS_ON );

  pinMode( DOUT_5V_ENABLE , OUTPUT );
  digitalWrite( DOUT_5V_ENABLE , STATE_5V_ON );

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

  bool test = busMaster.isDeviceAlive();

  Serial.println(test);

  Serial.println("test2");
  uint8_t vm_reg = busMaster.readWordLSB(0x5A, 0x00);

  Serial.println(vm_reg,BIN);

  Serial.println("test3");
  // delay(1000);

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

  Serial.println("DRV ready");
  delay(2000);
}

uint8_t effect = 16; //88!, 64, 16;

void loop() 
{
  uint8_t tmpByte;
  Serial.print("Effect #"); Serial.println(effect);


  // play the effect!

  uint8_t vm_reg = busMaster.readWordLSB(0x5A, 0x21);
  
  drv.go();

  

  Serial.print("supply voltage = ");

  float sup_volt = (float)vm_reg * 0.0219;
  Serial.println(sup_volt);
  // wait a bit
  

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

  delay(10000);

  /*
  effect++;
  if (effect > 117) effect = 1;
  */
}


uint8_t VIBRATION_MOTOR__read_status_register()
{
  uint8_t vm_reg = busMaster.readWordLSB(0x5A, 0x00);

  return vm_reg;
} 

void VIBRATION_MOTOR__check_driver_status() 
{
  uint8_t vm_reg = VIBRATION_MOTOR__read_status_register();

  Serial.println("vm_reg");
  Serial.println(vm_reg,BIN);

  (vm_reg & 0x01) == 0x01 ? _vibration_motor__reg_over_current_detect   = 1 : _vibration_motor__reg_over_current_detect = 0;

  (vm_reg & 0x02) == 0x02 ? _vibration_motor__reg_over_temp_detect      = 1 : _vibration_motor__reg_over_temp_detect = 0;

  (vm_reg & 0xE0) == 0xE0 ? _vibration_motor__reg_device_id_detect      = 1 : _vibration_motor__reg_device_id_detect = 0;

  _vibration_motor__reg_device_id_detect = 1 ? _vibration_motor__driver_available = 0xAA : _vibration_motor__driver_available = 0x55;
}

void scan()
{
    byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}



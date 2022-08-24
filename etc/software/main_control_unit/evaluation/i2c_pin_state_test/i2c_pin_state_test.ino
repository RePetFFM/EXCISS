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


  pinMode( DOUT_I2CMUX_SDA, INPUT );
  // digitalWrite( DOUT_I2CMUX_SDA , HIGH );

  pinMode( DOUT_I2CMUX_SCL , INPUT );
  // digitalWrite( DOUT_I2CMUX_SCL , HIGH );


  uint8_t  scl = digitalRead(DOUT_I2CMUX_SCL);
  uint8_t  sda = digitalRead(DOUT_I2CMUX_SDA);

  Serial.print("scl = ");
  Serial.println(scl);
  Serial.print("sda = ");
  Serial.println(sda);

  // busMaster = BusMaster();
  // busMaster.initBus();
  
}


void loop() 
{

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



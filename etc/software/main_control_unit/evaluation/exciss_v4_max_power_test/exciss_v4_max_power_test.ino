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

void setup() 
{
  // initilize BusMaster

  TCCR5B = TCCR5B & 0b11111000 | 0x01; // 0x01 = 31250Hz/1, 0x02 = 31250Hz/8

  Serial.begin(9600);

  delay(1000); // wait for console opening

  Serial.println("DRV test");



  pinMode( DOUT_VSYS_ENABLE, OUTPUT );
  digitalWrite( DOUT_VSYS_ENABLE , STATE_VSYS_ON );

  pinMode( DOUT_5V_ENABLE , OUTPUT );
  digitalWrite( DOUT_5V_ENABLE , STATE_5V_ON );

  

  pinMode( PWMOUT_FRONTLIGHT_CONTROL , OUTPUT);
  analogWrite( PWMOUT_FRONTLIGHT_CONTROL , 0);

  pinMode( PWMOUT_BACKLIGHT_CONTROL , OUTPUT);
  analogWrite( PWMOUT_BACKLIGHT_CONTROL , 0);


  pinMode( DOUT_MOT_ENABLE , OUTPUT );
  digitalWrite( DOUT_MOT_ENABLE , LOW );


  pinMode( DOUT_SCU_SD_TOGGLE , OUTPUT );
  digitalWrite( DOUT_SCU_SD_TOGGLE , STATE_SCU_SD_TOGGLE_XU1100 );

  pinMode( DOUT_SCU_PWR_ENABLE , OUTPUT );
  digitalWrite( DOUT_SCU_PWR_ENABLE , STATE_SCU_PWR_OFF );

  pinMode( DOUT_SCU_SHUTDOWN_REQUEST , OUTPUT );
  digitalWrite( DOUT_SCU_SHUTDOWN_REQUEST , STATE_SCU_RUN );


  pinMode( DOUT_USBHUB_PORT_1 , OUTPUT );
  pinMode( DOUT_USBHUB_PORT_2 , OUTPUT );
  digitalWrite( DOUT_USBHUB_PORT_2 , HIGH );

  pinMode( DOUT_USBHUB_RESET , STATE_USBHUB_NOT_RESET );
  digitalWrite( DOUT_USBHUB_RESET , LOW); 

  

  pinMode( DOUT_USBHUB_PWR , OUTPUT );
  digitalWrite( DOUT_USBHUB_PWR , STATE_USBHUB_PWR_ON );

  pinMode( DOUT_USB_X_PWR , OUTPUT );
  digitalWrite( DOUT_USB_X_PWR , STATE_USB_ON );

  pinMode( DOUT_USB_A_PWR , OUTPUT );
  digitalWrite( DOUT_USB_A_PWR , STATE_USB_ON );

  pinMode( DOUT_USB_B_PWR , OUTPUT );
  digitalWrite( DOUT_USB_B_PWR , STATE_USB_ON );


  pinMode( DOUT_USB_X_TOGGLE , OUTPUT );
  digitalWrite( DOUT_USB_X_TOGGLE , STATE_USB_X_TO_SCU );


  pinMode( DOUT_SCU_PWR_ENABLE , OUTPUT );
  digitalWrite( DOUT_SCU_PWR_ENABLE , STATE_SCU_PWR_ON );

  


  busMaster = BusMaster();
  busMaster.initBus();


  bool b = busMaster.switchMuxPortByDeviceAddress(0x5A); // DRV2605



  scan();

  
  delay(1000); // wait for console opening

  Serial.println("DRV go");


  drv.begin();
  drv.selectLibrary(1);
  
  // I2C trigger by sending 'go' command 
  // default, internal trigger when sending GO command
  drv.setMode(DRV2605_MODE_INTTRIG); 
  drv.useERM();
}

uint8_t effect = 16; //88!, 64, 16;

void loop() 
{

  Serial.print("Effect #"); Serial.println(effect);

  // set the effect to play
  drv.setWaveform(0, effect);  // play effect 
  drv.setWaveform(1, effect);       // end waveform
  drv.setWaveform(2, effect);       // end waveform
  drv.setWaveform(3, 0);       // end waveform

  // play the effect!
  //  drv.go();

  // wait a bit
  delay(3000);

  /*
  effect++;
  if (effect > 117) effect = 1;
  */
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



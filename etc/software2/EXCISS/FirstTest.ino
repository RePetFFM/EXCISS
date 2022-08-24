/*
 * First test of the chip with lib from:
 * http://ko7m.blogspot.de/2014/09/updated-pca9546-library.html
 * Rainer, 17.02.2018
 */

#include "PCA9546.h"
#include "RTClib.h"
#include <Wire.h>

#include <OneWire.h>
#include <DallasTemperature.h>

RTC_DS3231 rtc;
OneWire oneWire;
// OneWire oneWire(2) // Use address 20
DallasTemperature sensors(&oneWire);

void setup() 
{

  // HACK: use a pin to power the Mux Shield
  pinMode(A15, OUTPUT);
  digitalWrite(A15, HIGH);

  

  Serial.begin(9600);
  Serial.println("Starting...");
  PCA9546 pca(0x70);
  
  bool bSuccess = pca.selectChannel(0); // no channel
  Serial.print("No channel: ");
  Serial.println(bSuccess);
  Serial.println("");

  uint8_t temp = pca.i2c_read();
  Serial.print("i2c_read() : ");
  Serial.println(temp);



  //////////////////////////////////////////
  // PORT 1 = 0x1
  {
    bSuccess = pca.selectChannel(0x1);
    Serial.print("Channel ");
    Serial.print(0x1);
    Serial.print(": ");
    Serial.println(bSuccess);
  
    temp = pca.i2c_read();
    Serial.print("i2c_read() : ");
    Serial.println(temp);
  
    
    //checkRTC();
    
    scan();
    Serial.println("");

    readTemp();
  }
  



  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  //////////////////////////////////////////
  // PORT 2 = 0x1
  {
    bSuccess = pca.selectChannel(0x2);
    Serial.print("Channel ");
    Serial.print(0x2);
    Serial.print(": ");
    Serial.println(bSuccess);
  
    temp = pca.i2c_read();
    Serial.print("i2c_read() : ");
    Serial.println(temp);
  
    
    checkRTC();
    
    
    scan();
  }
  

  /*
  //////////////////////////////////////////
  // PORT 3 = 0x4
  {
    bSuccess = pca.selectChannel(0x4);
    Serial.print("Channel ");
    Serial.print(0x4);
    Serial.print(": ");
    Serial.println(bSuccess);
  
    temp = pca.i2c_read();
    Serial.print("i2c_read() : ");
    Serial.println(temp);
  
    
    checkRTC();
    
    scan();
  }



  //////////////////////////////////////////
  // PORT 4 = 0x8
  {
    bSuccess = pca.selectChannel(0x8);
    Serial.print("Channel ");
    Serial.print(0x8);
    Serial.print(": ");
    Serial.println(bSuccess);
  
    temp = pca.i2c_read();
    Serial.print("i2c_read() : ");
    Serial.println(temp);
  
    checkRTC();
    
    scan();
  }
  */
  
}



void readTemp()
{
    sensors.begin();
  
  DeviceAddress currAddress;
  uint8_t numberOfDevices = sensors.getDeviceCount();
  
  for (int i=0; i<numberOfDevices; i++)
  {
    sensors.getAddress(currAddress, i);
    printAddress(currAddress);
    Serial.println();  

  Serial.println("Requesting temperatures...");
  sensors.requestTemperatures();
  
  DeviceAddress currAddress;
  uint8_t numberOfDevices = sensors.getDeviceCount();
  
  for (int i=0; i<numberOfDevices; i++)
  {
    sensors.getAddress(currAddress, i);
    printAddress(currAddress);
    Serial.print(": ");
    Serial.print(sensors.getTempCByIndex(i));
    Serial.println();  
  }
    
  for(;;)
  {}
}
}





void printAddress(DeviceAddress deviceAddress)
{
  Serial.print("{ ");
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    Serial.print("0x");
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i<7) Serial.print(", ");
    
  }
  Serial.print(" }");
}


void checkRTC()
{
      
    //RTC_DS3231 rtc;
//    if (rtc.begin()) 
//    {
      //Serial.println("RTC found");
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      DateTime now = rtc.now();
      
      Serial.print(now.year(), DEC);
      Serial.print('-');
      Serial.print(now.month(), DEC);
      Serial.print('-');
      Serial.print(now.day(), DEC);
      Serial.print(' ');
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();
//    }
//    else
//    {
//      Serial.println("RTC NOT found");
//    }
  
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

void loop() 
{
  // put your main code here, to run repeatedly:
  delay(1000);
  checkRTC();
}


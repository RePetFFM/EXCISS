/***********************************************************************
* FILENAME :        Mux_OneWire_Temp_Test.cpp      DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Standalone test program to test the OneWire Bridge and the temperature
* sensors "behind" the I2C Muxer using the BusMaster library.
*
* Code for the OnwWire bridge:
* https://github.com/cybergibbons/DS2482_OneWire
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Rainer Haseitl
*
**/

#include "Mux_OneWire_Temp_Test_softi2cmaster.h"

#include "BusMaster.h"

#include "OneWire.h"
#include "DallasTemperature.h"

BusMaster busMaster;

// When instantiated with no parameters, uses I2C address 18
OneWire oneWire;
// OneWire oneWire(2) // Use address 20
DallasTemperature sensors(&oneWire);



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


void setup() 
{
  // initilize BusMaster
  busMaster = BusMaster();
  busMaster.initBus();

  bool b = busMaster.switchMuxPortByDeviceAddress(0x18); // this is where the Bridge is



  Serial.begin(19200);
  Serial.println("DS18B20 search");
  
  sensors.begin();
  
  DeviceAddress currAddress;
  uint8_t numberOfDevices = sensors.getDeviceCount();
  
  for (int i=0; i<numberOfDevices; i++)
  {
    sensors.getAddress(currAddress, i);
    printAddress(currAddress);
    Serial.println();  
  }
  
}

void loop() 
{
  
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
  
  delay(10000);
}


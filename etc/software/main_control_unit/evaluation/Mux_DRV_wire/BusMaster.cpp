/***********************************************************************
* FILENAME :        BusMaster.cpp                  DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Wrapper for i2c and SMBus communication.
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/

/**********************************************************************************
 * THIS FILE HAS SOME STUFF COMMENTED OUT BY RAINER TO MAKE IT RUN STANDLONE
 **********************************************************************************/
 
#include <Arduino.h>

//#include "exciss.h"

#include "BusMaster.h"

//#include "BQ25700A.h"
//#include "BQ40Z50.h"

#include <I2C.h>

struct MuxDevices MuxDevicesArray[] = {
  {0x5A, MUX_CHANNEL_1},                  // Vibration motor driver
  {0x18, MUX_CHANNEL_2},                  // Temperature OneWire
  {0x09, MUX_CHANNEL_3}, // Charger BQ25700A_DEFAULT_ADDR
  {0x0B, MUX_CHANNEL_3},  // Gauge BQ40Z50_DEFAULT_ADDR
  {0x68,MUX_CHANNEL_4}                    // RTC
};


BusMaster::BusMaster() {
}

bool BusMaster::initBus() {
  I2c.begin();

  I2c.setSpeed(0);

  I2c.pullup(0);

  I2c.timeOut(80);


  // return i2c_init();
}

bool BusMaster::isDeviceAlive()
{
  // I2c.scan();
  return true;
}

int BusMaster::readWordLSB(uint8_t aI2CDeviceAddress, uint8_t aFunction) {
  switchMuxPortByDeviceAddress(aI2CDeviceAddress);
  uint8_t busError = I2c.write(aI2CDeviceAddress,aFunction);


  if(busError > 0) {
    return busError;
  } else {
    I2c.read(aI2CDeviceAddress,2);
    uint8_t tLSB = I2c.receive();
    uint8_t tMSB = I2c.receive();
    delay(1);
    return (int) tLSB | (((int) tMSB) << 8);  
  }

  return 0;
}

void BusMaster::writeWordLSB(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint16_t aWord) {
  switchMuxPortByDeviceAddress(aI2CDeviceAddress);
  uint8_t arr[2];
  arr[0] = (uint8_t)aWord;
  arr[1] = aWord>>8;
  I2c.write(aI2CDeviceAddress, aFunction, arr, 2);
}

uint8_t BusMaster::readBlock(uint8_t aI2CDeviceAddress, uint8_t aCommand, uint8_t* aDataBufferPtr, uint8_t aDataBufferLength) {
  switchMuxPortByDeviceAddress(aI2CDeviceAddress);
  /*
  i2c_start((aI2CDeviceAddress << 1) + I2C_WRITE);
  i2c_write(aCommand);
  i2c_rep_start((aI2CDeviceAddress << 1) + I2C_READ);

  // First read length of data
  uint8_t tLengthOfData = i2c_read(false);
  if (tLengthOfData > aDataBufferLength) {
    tLengthOfData = aDataBufferLength;
  }

  // then read data
  uint8_t tIndex;
  for (tIndex = 0; tIndex < tLengthOfData - 1; tIndex++) {
    aDataBufferPtr[tIndex] = i2c_read(false);
  }
  // Read last byte with "true"
  aDataBufferPtr[tIndex++] = i2c_read(true);

  i2c_stop();
  */
  uint8_t tLengthOfData;
  return tLengthOfData;
}

bool BusMaster::switchMuxPortByDeviceAddress (uint8_t aRequestedDeviceAddress) {
  uint8_t tmpDeviceAddress;
  uint8_t tmpMuxPort;
  uint8_t i=0;

  

  bool switchDone = false;
  while(i<(sizeof(MuxDevicesArray)/sizeof(MuxDevices))) {
    tmpDeviceAddress = MuxDevicesArray[i].deviceAddress;
    tmpMuxPort = MuxDevicesArray[i].muxPort;
    if(tmpDeviceAddress == aRequestedDeviceAddress) {
      _currentMuxChannel != tmpMuxPort ? I2c.write(MUX_DEVICE_ADDRESS, tmpMuxPort) : true; // is switching required?
      switchDone = true;
      break;
    }
    i++;
  }
  
  return switchDone;
}


void BusMaster::writeByte(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint8_t aByte) {
  switchMuxPortByDeviceAddress(aI2CDeviceAddress);
  I2c.write(aI2CDeviceAddress, aFunction, aByte);
}

void BusMaster::writeByte(uint8_t aI2CDeviceAddress, uint8_t aByte) {
  switchMuxPortByDeviceAddress(aI2CDeviceAddress);
  I2c.write(aI2CDeviceAddress, aByte);
}


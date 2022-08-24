/***********************************************************************
* FILENAME :        BusMaster.h                    DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Wrapper for i2c and SMBus communication.
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/

#ifndef _BusMaster_H_
#define _BusMaster_H_

#include <Arduino.h>



struct MuxDevices {
  uint8_t deviceAddress;
  uint8_t muxPort;
};

#define MUX_DEVICE_ADDRESS 0x70

#define MUX_CHANNEL_NONE 0x0
#define MUX_CHANNEL_1 0x1
#define MUX_CHANNEL_2 0x2
#define MUX_CHANNEL_3 0x4
#define MUX_CHANNEL_4 0x8


class BusMaster
{
public:
  BusMaster();

  bool initBus();

  int readWordLSB(uint8_t aI2CDeviceAddress, uint8_t aFunction);

  void writeWordLSB(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint16_t aWord);

  void writeByte(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint8_t aByte);

  void writeByte(uint8_t aI2CDeviceAddress, uint8_t aByte);

  uint8_t readBlock(uint8_t aI2CDeviceAddress, uint8_t aCommand, uint8_t* aDataBufferPtr, uint8_t aDataBufferLength);

  bool switchMuxPortByDeviceAddress (uint8_t aRequestedDeviceAddress);

protected:

private: 
  uint8_t _currentMuxChannel = 0x00;

};


#endif  // _BusMaster_H_


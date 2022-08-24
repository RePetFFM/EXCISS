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
// #include "exciss.h"



struct MuxDevices {
	uint8_t deviceAddress;
	uint8_t muxPort;
	uint8_t locked; // 0x55 = not locked, 0xAA = locked
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

	void autoSwitching(bool aEnable);

	void resetSwitch();

	int readWordLSB(uint8_t aI2CDeviceAddress, uint8_t aFunction);

	int readWordLSB_no_interrupt_no_lockup(uint8_t aI2CDeviceAddress, uint8_t aFunction);

	uint8_t readByte(uint8_t aI2CDeviceAddress, uint8_t aFunction);

	uint8_t readByte(uint8_t aI2CDeviceAddress, uint8_t aCommand, uint8_t* aDataBufferPtr, uint8_t aDataBufferLength);

	void writeWordLSB(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint16_t aWord);

	uint8_t write(uint8_t aI2CDeviceAddress, uint8_t registerAddress, uint8_t *data, uint8_t numberBytes);

	void write(uint8_t , uint16_t ); 

	void writeByte(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint8_t aByte);

	void writeByte_no_interrupt_no_lockup(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint8_t aByte);

	void writeByte(uint8_t aI2CDeviceAddress, uint8_t aByte);

	uint8_t readBlock(uint8_t aI2CDeviceAddress, uint8_t aCommand, uint8_t* aDataBufferPtr, uint8_t aDataBufferLength);

	uint8_t switchMuxPortByDeviceAddress (uint8_t aRequestedDeviceAddress);

	void lockDeviceByAddress (uint8_t aDeviceAddress);
	
	void unlockDeviceByAddress (uint8_t aDeviceAddress);

	void logoutI2CLockState ();

	

protected:

private: 

};


#endif	// _BusMaster_H_
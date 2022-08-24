/***********************************************************************
* FILENAME :        BusMaster.cpp                  DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Wrapper for i2c and SMBus communication.
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
* Requires: http://dsscircuits.com/index.php/articles/66-arduino-i2c-master-library
**/

#include <Arduino.h>

#include "exciss.h"

#include "BusMaster.h"

#include "BQ25700A.h"

#include "BQ40Z50.h"

#include <I2C.h>


struct MuxDevices MuxDevicesArray[] = {
  {0x5A, MUX_CHANNEL_1},                  // Vibration motor driver
  {0x18, MUX_CHANNEL_2},                  // Temperature OneWire
  {BQ25700A_DEFAULT_ADDR, MUX_CHANNEL_3}, // Charger
  {BQ40Z50_DEFAULT_ADDR, MUX_CHANNEL_3},  // Gauge
  {0x68,MUX_CHANNEL_4}                    // RTC
};

uint8_t _busmaster_auto_switch = 0xAA;


BusMaster::BusMaster() {
}

bool BusMaster::initBus() {

    // init i2c mux reset port
    pinMode(DOUT_I2CMUX_RST ,OUTPUT);

    resetSwitch();

    I2c.begin();

    I2c.setSpeed(0);

    I2c.pullup(0);

    I2c.timeOut(80);
    // return i2c_init();
}

void BusMaster::autoSwitching(bool aEnable)
{
    aEnable ? _busmaster_auto_switch = 0xAA : _busmaster_auto_switch = 0x55;
}

void BusMaster::resetSwitch() 
{
    digitalWrite(DOUT_I2CMUX_RST , STATE_BUSMASTER_RESET_DO_RESET);
    // delay(1);
    digitalWrite(DOUT_I2CMUX_RST , STATE_BUSMASTER_RESET_NORMAL);
    // delay(1);
}

int BusMaster::readWordLSB(uint8_t aI2CDeviceAddress, uint8_t aFunction) 
{
    cli();
    switchMuxPortByDeviceAddress(aI2CDeviceAddress);
    I2c.write(aI2CDeviceAddress,aFunction);
    I2c.read(aI2CDeviceAddress,2);
    uint8_t tLSB = I2c.receive();
    uint8_t tMSB = I2c.receive();
    sei();
    delay(1);
    return (int) tLSB | (((int) tMSB) << 8);
}

uint8_t BusMaster::readByte(uint8_t aI2CDeviceAddress, uint8_t aFunction) 
{
    cli();
    switchMuxPortByDeviceAddress(aI2CDeviceAddress);
    I2c.write(aI2CDeviceAddress,aFunction);
    I2c.read(aI2CDeviceAddress,1);
    uint8_t res = I2c.receive();
    sei();
    return res;
}

void BusMaster::writeWordLSB(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint16_t aWord) {
    cli();
    switchMuxPortByDeviceAddress(aI2CDeviceAddress);
    uint8_t arr[2];
    arr[0] = (uint8_t)aWord;
    arr[1] = aWord>>8;
    I2c.write(aI2CDeviceAddress, aFunction, arr, 2);
    sei();
}

uint8_t BusMaster::readBlock(uint8_t aI2CDeviceAddress, uint8_t aCommand, uint8_t* aDataBufferPtr, uint8_t aDataBufferLength) {
    switchMuxPortByDeviceAddress(aI2CDeviceAddress);


    //read(uint8_t address, uint8_t registerAddress, uint8_t numberBytes)
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


uint8_t BusMaster::write(uint8_t aI2CDeviceAddress, uint8_t registerAddress, uint8_t *data, uint8_t numberBytes)
{
    cli();
    switchMuxPortByDeviceAddress(aI2CDeviceAddress);

    I2c.write(aI2CDeviceAddress, registerAddress, data, numberBytes);
    sei();
}

void BusMaster::write(uint8_t aI2CDeviceAddress, uint16_t aFunction) 
{
    cli();
    switchMuxPortByDeviceAddress(aI2CDeviceAddress);
    I2c.write(aI2CDeviceAddress, aFunction);
    sei();
}


bool BusMaster::switchMuxPortByDeviceAddress (uint8_t aRequestedDeviceAddress) {
    uint8_t tmpDeviceAddress;
    uint8_t tmpMuxPort;
    uint8_t i=0;

    bool switchDone = false;

    if(_busmaster_auto_switch == 0xAA) 

    {

        resetSwitch();

        
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

    } 
    else 
    {
        switchDone = true;
    }

    _busmaster_auto_switch = 0xAA; // set to default value
    
    
    return switchDone;
}


void BusMaster::writeByte(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint8_t aByte) {
    cli();
    switchMuxPortByDeviceAddress(aI2CDeviceAddress);
    I2c.write(aI2CDeviceAddress, aFunction, aByte);
    sei();
}

void BusMaster::writeByte(uint8_t aI2CDeviceAddress, uint8_t aByte) {
    cli();
    switchMuxPortByDeviceAddress(aI2CDeviceAddress);
    I2c.write(aI2CDeviceAddress, aByte);
    sei();
}
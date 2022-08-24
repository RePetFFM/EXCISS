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


#define I2C_PULLUP 0

#define SDA_PORT PORTD
#define SDA_PIN 1

#define SCL_PORT PORTD
#define SCL_PIN 0
#define I2C_SLOWMODE 1

#define I2C_TIMEOUT 10
#define I2C_MAXWAIT 10
// Otherwise it may give read errors because of the arduino 1 ms clock interrupt.
#define I2C_NOINTERRUPT  0
#define I2C_HARDWARE 0

#include <SoftI2CMaster.h>

#define DATA_BUFFER_LENGTH 32
uint8_t sI2CDataBuffer[DATA_BUFFER_LENGTH];

/*
cheatsheet i2c 6bit address
mux         E0
driver      B4
oneWBridge  30
BQ25700A    12
BQ40Z50     16
RTC         D0
*/

struct MuxDevices MuxDevicesArray[] = {
  {0x5A, MUX_CHANNEL_1, 0x55},                  // Vibration motor driver
  {0x18, MUX_CHANNEL_2, 0x55},                  // Temperature OneWire
  {BQ25700A_DEFAULT_ADDR, MUX_CHANNEL_3, 0x55}, // Charger
  {BQ40Z50_DEFAULT_ADDR, MUX_CHANNEL_3, 0x55},  // Gauge
  {0x68,MUX_CHANNEL_4, 0x55}                    // RTC
};

// test mux setting.
/*
struct MuxDevices MuxDevicesArray[] = {
  {0x5A, MUX_CHANNEL_3},                  // Vibration motor driver
  {0x18, MUX_CHANNEL_1},                  // Temperature OneWire
  {BQ25700A_DEFAULT_ADDR, MUX_CHANNEL_4}, // Charger
  {BQ40Z50_DEFAULT_ADDR, MUX_CHANNEL_4},  // Gauge
  {0x68,MUX_CHANNEL_2}                    // RTC
};
*/

uint8_t _busmaster_auto_switch = 0xAA;

bool    _busmaster__locked_i2c_dev_mux = 0x55;

uint8_t _busmaster__bool_no  = 0x55;
uint8_t _busmaster__bool_yes = 0xAA;

BusMaster::BusMaster() {
}

bool BusMaster::initBus() {

    _busmaster__bool_no  = 0x55;
    _busmaster__bool_yes = 0xAA;

    MuxDevicesArray[0].locked = _busmaster__bool_no;
    MuxDevicesArray[1].locked = _busmaster__bool_no;
    MuxDevicesArray[2].locked = _busmaster__bool_no;
    MuxDevicesArray[3].locked = _busmaster__bool_no;
    MuxDevicesArray[4].locked = _busmaster__bool_no;

    // init i2c mux reset port
    pinMode(DOUT_I2CMUX_RST ,OUTPUT);

    resetSwitch();

    bool tI2CSucessfullyInitialized = i2c_init();

    if (tI2CSucessfullyInitialized) {
        com.log(F("I2C initalized sucessfully"));
        com.newline();
    } else {
        com.log(F("I2C pullups missing"));
        com.newline();
    }
    
    return tI2CSucessfullyInitialized;
}

void BusMaster::autoSwitching(bool aEnable)
{
    aEnable ? _busmaster_auto_switch = _busmaster__bool_yes : _busmaster_auto_switch = _busmaster__bool_no;
}

void BusMaster::resetSwitch() 
{

    digitalWrite(DOUT_I2CMUX_RST , STATE_BUSMASTER_RESET_DO_RESET);
    delayMicroseconds(50);
    digitalWrite(DOUT_I2CMUX_RST , STATE_BUSMASTER_RESET_NORMAL);
    delayMicroseconds(50);
}

int BusMaster::readWordLSB(uint8_t aI2CDeviceAddress, uint8_t aFunction) 
{
    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);
    uint8_t tLSB = 0;
    uint8_t tMSB = 0;

    if( locked_device == _busmaster__bool_no ) 
    {

        cli();
        if( i2c_start_wait((aI2CDeviceAddress << 1) | I2C_WRITE) )
        {

            i2c_write(aFunction);
            i2c_rep_start((aI2CDeviceAddress << 1) | I2C_READ);
            tLSB = i2c_read(false);
            tMSB = i2c_read(true);

        } else {
            lockDeviceByAddress(aI2CDeviceAddress);
        }
        i2c_stop();
        sei();
        return (int) tLSB | (((int) tMSB) << 8);
    } else {
        return (int) tLSB | (((int) tMSB) << 8);
    }
    
}


int BusMaster::readWordLSB_no_interrupt_no_lockup(uint8_t aI2CDeviceAddress, uint8_t aFunction) 
{
    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);
    uint8_t tLSB = 0;
    uint8_t tMSB = 0;

    if( locked_device == _busmaster__bool_no ) 
    {

        if( i2c_start_wait((aI2CDeviceAddress << 1) | I2C_WRITE) )
        {

            i2c_write(aFunction);
            i2c_rep_start((aI2CDeviceAddress << 1) | I2C_READ);
            tLSB = i2c_read(false);
            tMSB = i2c_read(true);

        }
        i2c_stop();
        
        return (int) tLSB | (((int) tMSB) << 8);
    } else {
        return (int) tLSB | (((int) tMSB) << 8);
    }
    
}


uint8_t BusMaster::readByte(uint8_t aI2CDeviceAddress, uint8_t aFunction) 
{
    
    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);
    uint8_t res = 0;

    if( locked_device == _busmaster__bool_no ) 
    {
        cli();
        if( i2c_start_wait((aI2CDeviceAddress << 1) | I2C_WRITE) )
        {
            i2c_write(aFunction);
            i2c_rep_start((aI2CDeviceAddress << 1) | I2C_READ);
            res = i2c_read(true);
        } else {
            lockDeviceByAddress(aI2CDeviceAddress);
        }
        i2c_stop();
        sei();
        return res;
    }
    return 0;
}


uint8_t BusMaster::readByte(uint8_t aI2CDeviceAddress, uint8_t aCommand, uint8_t* aDataBufferPtr, uint8_t aDataBufferLength) {

    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);
    uint8_t tLengthOfData = 0;

    if( locked_device == _busmaster__bool_no ) 
    {
        cli();
        if( i2c_start_wait((aI2CDeviceAddress << 1) + I2C_WRITE) )
        {
            i2c_write(aCommand);
            i2c_rep_start((aI2CDeviceAddress << 1) + I2C_READ);
            tLengthOfData = aDataBufferLength;

            // then read data
            uint8_t tIndex;
            for (tIndex = 0; tIndex < tLengthOfData-1; tIndex++) {
                aDataBufferPtr[tIndex] = i2c_read(false);
            }
            // Read last byte with "true"
            aDataBufferPtr[tIndex++] = i2c_read(true);    
        } else {
            lockDeviceByAddress(aI2CDeviceAddress);
        }
        

        i2c_stop();
        sei();
    }   
    return tLengthOfData;
}


void BusMaster::writeWordLSB(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint16_t aWord) {
    
    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);

    if( locked_device == _busmaster__bool_no ) 
    {
        cli();
        if( i2c_start_wait((aI2CDeviceAddress << 1) | I2C_WRITE) )
        {
            i2c_write(aFunction);
            i2c_write(aWord & 0xFF);
            i2c_write((aWord >> 8) & 0xFF);
        } else {
            lockDeviceByAddress(aI2CDeviceAddress);
        }

        i2c_stop();
        sei();
    }
}



uint8_t BusMaster::readBlock(uint8_t aI2CDeviceAddress, uint8_t aCommand, uint8_t* aDataBufferPtr, uint8_t aDataBufferLength) {

    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);

    uint8_t tLengthOfData = 0;

    if( locked_device == _busmaster__bool_no ) 
    {
        cli();
        if( i2c_start_wait((aI2CDeviceAddress << 1) + I2C_WRITE) )
        {
            i2c_write(aCommand);
            i2c_rep_start((aI2CDeviceAddress << 1) + I2C_READ);

            // First read length of data
            tLengthOfData = i2c_read(false);
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

        } else {
            lockDeviceByAddress(aI2CDeviceAddress);
        }

        i2c_stop();
        sei();
    }
    return tLengthOfData;
}


uint8_t BusMaster::write(uint8_t aI2CDeviceAddress, uint8_t registerAddress, uint8_t *data, uint8_t numberBytes)
{
    
    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);

    if( locked_device == _busmaster__bool_no ) 
    {
        cli();
        if( i2c_start_wait((aI2CDeviceAddress << 1) | I2C_WRITE) )
        {
            i2c_write(registerAddress);

            for(uint8_t i=0; i<numberBytes; i++)
            {
                i2c_write(data[i]);    
            }

        } else {
            lockDeviceByAddress(aI2CDeviceAddress);
        }

        i2c_stop();
        sei();
    }
}


void BusMaster::write(uint8_t aI2CDeviceAddress, uint16_t aFunction) 
{
    
    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);

    if( locked_device == _busmaster__bool_no ) 
    {
        cli();
        if( i2c_start_wait((aI2CDeviceAddress << 1) | I2C_WRITE) )
        {
        
            i2c_write((aFunction >> 8) & 0xFF);
            i2c_write(aFunction & 0xFF);
            

        } else {
            lockDeviceByAddress(aI2CDeviceAddress);
        }
        i2c_stop();
        sei();
    }
}


void BusMaster::writeByte(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint8_t aByte) 
{
    
    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);

    if( locked_device == _busmaster__bool_no ) 
    {
        cli();
        if( i2c_start_wait((aI2CDeviceAddress << 1) | I2C_WRITE) )
        {
            i2c_write(aFunction);
            i2c_write(aByte);
            
        } else {
            lockDeviceByAddress(aI2CDeviceAddress);
        }
        i2c_stop();
        sei();
    }
}

void BusMaster::writeByte_no_interrupt_no_lockup(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint8_t aByte)
{
    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);

    if( locked_device == _busmaster__bool_no ) 
    {
        if( i2c_start_wait((aI2CDeviceAddress << 1) | I2C_WRITE) )
        {
            i2c_write(aFunction);
            i2c_write(aByte);
        } 

        i2c_stop();
    }
}


void BusMaster::writeByte(uint8_t aI2CDeviceAddress, uint8_t aByte) {
    
    uint8_t locked_device = switchMuxPortByDeviceAddress(aI2CDeviceAddress);

    if( locked_device == _busmaster__bool_no ) 
    {
        cli();
        if( i2c_start_wait((aI2CDeviceAddress << 1) | I2C_WRITE) )
        {
            i2c_write(aByte);
            
        } else {
            lockDeviceByAddress(aI2CDeviceAddress);
        }
        i2c_stop();
        sei();
    }
}


uint8_t BusMaster::switchMuxPortByDeviceAddress (uint8_t aRequestedDeviceAddress) {
    uint8_t tmpDeviceAddress;
    uint8_t tmpMuxPort;
    uint8_t i=0;

    uint8_t locked_device = _busmaster__bool_no;

    wdt_reset();

    if( _busmaster_auto_switch == _busmaster__bool_yes ) 

    {
        resetSwitch();
        
        while(i<5) {
            tmpDeviceAddress    = MuxDevicesArray[i].deviceAddress;
            tmpMuxPort          = MuxDevicesArray[i].muxPort;
            locked_device       = MuxDevicesArray[i].locked;
            if(tmpDeviceAddress == aRequestedDeviceAddress) {
                
                cli();
                if(i2c_start_wait((MUX_DEVICE_ADDRESS << 1) | I2C_WRITE)) {
                    i2c_write(tmpMuxPort);
                    _busmaster__locked_i2c_dev_mux = _busmaster__bool_no;
                } else {
                    _busmaster__locked_i2c_dev_mux = _busmaster__bool_yes;
                }
                i2c_stop();
                sei();
                
                break;
            }
            i++;
        }

    } 
    
    delayMicroseconds(50);

    _busmaster_auto_switch = _busmaster__bool_yes; // set to default value
    
    wdt_reset();
    
    return locked_device;
}


void BusMaster::lockDeviceByAddress (uint8_t aDeviceAddress)
{
    uint8_t tmpDeviceAddress;
    uint8_t i = 0;
    while(i<5) {
        tmpDeviceAddress    = MuxDevicesArray[i].deviceAddress;
        
        if(tmpDeviceAddress == aDeviceAddress) {

            MuxDevicesArray[i].locked = _busmaster__bool_yes;
            
            break;
        }
        i++;
    }    
}


void BusMaster::unlockDeviceByAddress (uint8_t aDeviceAddress)
{
    uint8_t tmpDeviceAddress;
    uint8_t i = 0;
    while(i<5) {
        tmpDeviceAddress    = MuxDevicesArray[i].deviceAddress;
        
        if(tmpDeviceAddress == aDeviceAddress) {

            MuxDevicesArray[i].locked = _busmaster__bool_no;
            
            break;
        }
        i++;
    }    
}


void BusMaster::logoutI2CLockState ()
{
    uint8_t tmpDeviceAddress;
    uint8_t i = 0;

    com.log("[ BusMaster ]");
    com.newline();

    while(i<5) 
    {
        tmpDeviceAddress = MuxDevicesArray[i].deviceAddress;

        com.log("i2c_address = ");
        com.logHEX( tmpDeviceAddress );
        com.newline();
        com.log("i2c_lock_status = ");
        com.logHEX( MuxDevicesArray[i].locked );
        com.newline();
        
        i++;
    }       

}
/***********************************************************************
* DESCRIPTION :
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/


#include "SPI_EEPROM.h"



SPI_EEPROM::SPI_EEPROM() {
    init_ports(SPI_EEPROM_PORT_SLAVESELECT, SPI_EEPROM_PORT_WRITEPROTECT, SPI_EEPROM_PORT_HOLD);
}

SPI_EEPROM::SPI_EEPROM(int a_eeprom_select_port, int a_eeprom_writeprotect_port, int a_eeprom_hold_port ) {
    init_ports(a_eeprom_select_port, a_eeprom_writeprotect_port, a_eeprom_hold_port );
}

void SPI_EEPROM::init_ports(int a_eeprom_select_port, int a_eeprom_writeprotect_port, int a_eeprom_hold_port ) {

    SPI.begin();
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));


    pinMode(SPI_EEPROM_PORT_DATAOUT, OUTPUT);
    pinMode(SPI_EEPROM_PORT_DATAIN, INPUT);
    pinMode(SPI_EEPROM_PORT_SPICLOCK,OUTPUT);
    pinMode(SPI_EEPROM_PORT_HOLD,OUTPUT);
    pinMode(SPI_EEPROM_PORT_WRITEPROTECT,OUTPUT);
    pinMode(SPI_EEPROM_PORT_SLAVESELECT,OUTPUT);
    digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,HIGH); //disable device
    digitalWrite(SPI_EEPROM_PORT_HOLD,HIGH);
    digitalWrite(SPI_EEPROM_PORT_WRITEPROTECT,HIGH);
}

bool SPI_EEPROM::writeBytes(uint16_t a_address, char * a_byte, int length) {
    unsigned long timeout_time = millis() + SPI_EEPROM_TIMEOUT_WRITE_WAIT; // set timeout time until 
    bool timedout = false;

    // wait until eeprom is ready to execute write operation
    while(1) {

        // check timeout condition and break out while loop if timedout
        if( timeout_time <= millis() ) {
            timedout = true; // set the timedout flag to prevent starting writing byte
            break;
        }

        // check WIP flag in status register. If flag is set the eeprom is busy
        if(write_ready()) {
            break;
        }
    }

    // if not timedout start write operations
    if( !timedout ) {
        digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,LOW);
        SPI.transfer(SPI_EEPROM_OPCODE_WRITE);
        SPI.transfer((byte)(a_address>>8));   //send MSByte address first
        SPI.transfer((byte)(a_address));      //send LSByte address
        for(byte i=0; i<length; i++) {
            SPI.transfer(a_byte[i]);
        }
        
        digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,HIGH);
    }

    return !timedout;
}

bool SPI_EEPROM::readBytes(uint16_t a_address, char * a_buf, int length) {
    unsigned long timeout_time = millis() + SPI_EEPROM_TIMEOUT_WRITE_WAIT; // set timeout time until 
    bool timedout = false;
    //char returnValue = 0x00;

    // wait until eeprom is ready to execute write operation
    while(1) {

        // check timeout condition and break out while loop if timedout
        if( timeout_time <= millis() ) {
            timedout = true; // set the timedout flag to prevent starting writing byte
            break;
        }

        // check WIP flag in status register. If flag is set the eeprom is busy
        if(write_ready()) {
            break;
        }
    }



    // if not timedout start write operations
    if( !timedout ) {
        digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,LOW);
        SPI.transfer(SPI_EEPROM_OPCODE_READ);
        SPI.transfer((byte)(a_address>>8));     // send MSByte address first
        SPI.transfer((byte)(a_address));        // send LSByte address
        for(byte i=0; i<length; i++) {
            a_buf[i] = SPI.transfer(0xFF);
        }
        // *a_buf = SPI.transfer(0xFF);                     // send something to generate clock signal for one byte
        digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,HIGH);
    }

    // *a_buf = 0x33;

    return !timedout;
}

void SPI_EEPROM::write_enable() {
    digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,LOW);
    SPI.transfer(SPI_EEPROM_OPCODE_WREN);
    digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,HIGH);
}

struct SPI_EEPROM_CHIP_ID SPI_EEPROM::readChipID() {
    struct SPI_EEPROM_CHIP_ID returnVal;

    digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,LOW);

    SPI.transfer(SPI_EEPROM_OPCODE_RDID);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    returnVal.manufacturer_code = SPI.transfer(0xFF);
    returnVal.spi_family_code = SPI.transfer(0xFF);
    returnVal.memory_density_code = SPI.transfer(0xFF);
    
    digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,HIGH);

    return returnVal;
}

bool SPI_EEPROM::write_ready() {
    bool resultVal = false;
    char spi_return;
    digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,LOW);
    SPI.transfer(SPI_EEPROM_OPCODE_RDSR);
    spi_return = SPI.transfer(0xFF);
    digitalWrite(SPI_EEPROM_PORT_SLAVESELECT,HIGH);

    (spi_return & 0x01) == 0x01 ? resultVal = false : resultVal = true;

    return resultVal;
}
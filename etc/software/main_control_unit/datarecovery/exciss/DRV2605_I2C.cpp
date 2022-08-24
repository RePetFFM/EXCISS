/*************************************************** 
  This is a library for the Adafruit DRV2605L Haptic Driver

  ----> http://www.adafruit.com/products/2306
 
  Check out the links above for our tutorials and wiring diagrams
  This motor/haptic driver uses I2C to communicate

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "exciss.h"

#include "DRV2605_I2C.h"

/**************************************************************************/
/*! 
    @brief  Instantiates a new DRV2605 class
*/
/**************************************************************************/
// I2C, no address adjustments or pins
DRV2605::DRV2605() {
}


/**************************************************************************/
/*! 
    @brief  Setups the HW
*/
/**************************************************************************/
boolean DRV2605::begin() {

  uint8_t id = readRegister8(DRV2605_REG_STATUS);

  
  writeRegister8(DRV2605_REG_MODE, 0x00); // out of standby
  
  writeRegister8(DRV2605_REG_RTPIN, 0x00); // no real-time-playback
  
  
  writeRegister8(DRV2605_REG_OVERDRIVE, 0); // no overdrive
  
  writeRegister8(DRV2605_REG_SUSTAINPOS, 0);
  writeRegister8(DRV2605_REG_SUSTAINNEG, 0);
  writeRegister8(DRV2605_REG_BREAK, 0);
  writeRegister8(DRV2605_REG_AUDIOMAX, 0x64);
  
  // ERM open loop
  
  // turn off N_ERM_LRA
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
  // turn on ERM_OPEN_LOOP
  writeRegister8(DRV2605_REG_CONTROL3, readRegister8(DRV2605_REG_CONTROL3) | 0x20);

  return true;
}

void DRV2605::setWaveform(uint8_t slot, uint8_t w) {
  writeRegister8(DRV2605_REG_WAVESEQ1+slot, w);
}

void DRV2605::selectLibrary(uint8_t lib) {
  writeRegister8(DRV2605_REG_LIBRARY, lib);
}

void DRV2605::go() {
  writeRegister8(DRV2605_REG_GO, 1);
}

void DRV2605::go_noI_noL() 
{
  // Special variantion of go function. 
  // Prevents interrupt deactivation and lockup if device is not responding imediatly.
  
  busMaster.writeByte_no_interrupt_no_lockup(DRV2605_ADDR, DRV2605_REG_GO, 1);
}

void DRV2605::stop() {
  writeRegister8(DRV2605_REG_GO, 0);
}

void DRV2605::setMode(uint8_t mode) {
  writeRegister8(DRV2605_REG_MODE, mode);
}

void DRV2605::setRealtimeValue(uint8_t rtp) {
  writeRegister8(DRV2605_REG_RTPIN, rtp);
}

/********************************************************************/

uint8_t DRV2605::readRegister8(uint8_t reg) {
  uint8_t retByte ;

  retByte = busMaster.readByte(DRV2605_ADDR, reg);

  return retByte;
}

void DRV2605::writeRegister8(uint8_t reg, uint8_t val) {

    busMaster.writeByte(DRV2605_ADDR, reg, val);

}

/****************/


// Allow users to use ERM motor or LRA motors

void DRV2605::useERM ()
{
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
}

void DRV2605::useLRA ()
{
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) | 0x80);
}




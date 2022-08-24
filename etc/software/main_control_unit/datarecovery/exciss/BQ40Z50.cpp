/***********************************************************************
* FILENAME :        BQ40Z50.cpp                    DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Library for the smart batterypack manger BQ40Z50 from Texas Instruments.
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/

#include <Arduino.h>

#include "exciss.h"

#include "BQ40Z50.h"

// #include "BusMaster.h" 


BQ40Z50::BQ40Z50 () {

}

bool BQ40Z50::init(){

	return true;
}


void BQ40Z50::updateValues() {
	uint16_t tmpWord;
	uint16_t tmpVoltage;

	batteryMode = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_BATTERY_MODE);

	
	maxError = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_MAX_ERROR);


	voltage = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_VOLTAGE);


	current = (int16_t)busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_CURRENT);

	// batteryChemID = readManufacturerBlock(BQ40Z50_DEFAULT_ADDR, BQ40Z50_MAC, 0x0006);

	remainingCurrent = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_REMAINING_CAPACITY);

	
	fullChargCapacity = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_FULL_CHARGE_CAPACITY);

	
	minToFull = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_TIME_TO_FULL);

	
	minToEmpty = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_TIME_TO_EMPTY);

	
	minRuntime = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_RUNTIME_TO_EMPTY);

	
	temperature = ((float)busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_TEMPERATURE) * 0.1) - 273.15; // 0.1 kelvin steps

	
	chargeRelative = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_RELATIVE_CHARGE);

	
	chargeAbsolute = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_ABSOLUTE_CHARGE);

	
	batteryStatus = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_BATTERY_STATUS);

	
	cycleCount = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_CYCLE_COUNT);

	
	cell1voltage = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_CELL_1_VOLTAGE);

	
	cell2voltage = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_CELL_2_VOLTAGE);

	
	cell3voltage = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_CELL_3_VOLTAGE);

	
	cell4voltage = busMaster.readWordLSB(BQ40Z50_DEFAULT_ADDR, BQ40Z50_CELL_4_VOLTAGE);
}


uint16_t BQ40Z50::readWordManufacturer(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint16_t aManufacturerFunction) {
	uint16_t tmpWord;
	// busMaster.readWord
	/*
	i2c_start((sI2CDeviceAddress << 1) | I2C_WRITE);
	i2c_write(MANUFACTURER_ACCESS);
	*/

	// busMaster.writeByte(aI2CDeviceAddress,aFunction);
	// Write manufacturer command word
	/*
	i2c_rep_start((sI2CDeviceAddress << 1) | I2C_WRITE);
	i2c_write(aCommand);
	i2c_write(aCommand >> 8);
	i2c_stop();
	*/


	busMaster.autoSwitching(true);
	busMaster.writeByte(aI2CDeviceAddress,BQ40Z50_MAC);

	busMaster.autoSwitching(false);
    busMaster.write(aI2CDeviceAddress, aManufacturerFunction);
    
    busMaster.autoSwitching(false);
    tmpWord = busMaster.readWordLSB(aI2CDeviceAddress,BQ40Z50_MAC);

	// tmpWord = busMaster.readWordLSB(aI2CDeviceAddress,aFunction);
	// Read manufacturer result word
	/*
	i2c_start((sI2CDeviceAddress << 1) | I2C_WRITE);
	i2c_write(MANUFACTURER_ACCESS);
	i2c_rep_start((sI2CDeviceAddress << 1) | I2C_READ);
	uint8_t tLSB = i2c_read(false);
	uint8_t tMSB = i2c_read(true);
	i2c_stop();
	return (int) tLSB | (((int) tMSB) << 8);
	*/

	return tmpWord;
}
/***********************************************************************
* FILENAME :        BQ25700A.cpp                   DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Library for charger ic BQ2500A from Texas Instruments.
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/

#include <Arduino.h>

#include "exciss.h"

#include "BQ25700A.h"

// #include "BusMaster.h"


/*
BQ25700A::BQ25700A(uint8_t address) {
	// busMaster = BusMaster();
}*/

BQ25700A::BQ25700A() {
}

bool BQ25700A::init(){
	// bool tmpBool = busMaster.initDevice(BQ25700A_DEFAULT_ADDR);
	// int tmp = busMaster.readWordLSB(BQ25700A_REG_CHARGE_OPTION_0);
	enableADC();
	return true;
}


void BQ25700A::enableADC() {
	busMaster.writeWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_ADC_OPTION, 0xA0FF);
}

void BQ25700A::updateValues() {
	uint16_t tmpWord;
	uint16_t tmpVoltage;

	enableADC();

	delay(12);

	tmpWord = busMaster.readWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_ADC_VBUS_PSYS);
	adcVBUS = ((tmpWord>>8)*64)+3200;

	adcPSYS = (((uint8_t)tmpWord)*12)+3060;

	tmpWord = busMaster.readWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_ADC_IBAT);

	adcIBATCharge = (tmpWord>>8)*64;

	adcIBATDischarge = ((uint8_t)tmpWord)*256;

	tmpWord = busMaster.readWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_ADC_IIN_CMPIN);
	adcIIN = (tmpWord>>8)*50;

	tmpWord = busMaster.readWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_ADC_VSYS_VBAT);
	adcVSYS = ((tmpWord>>8)*64)+2880;

	adcVBAT = (((uint8_t)tmpWord)*64)+2880;

	// read current limit setting 
	tmpWord = busMaster.readWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_IIN_LIMIT);
	currentLimit = (tmpWord>>8)*50;

	adcOption = busMaster.readWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_ADC_OPTION);

	chargeOption = busMaster.readWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_CHARGE_OPTION_0);

	chargeCurrent = busMaster.readWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_CHARGECURRENT);

}

uint16_t BQ25700A::setImputCurrent(uint16_t aCurrent_mAh) {
	aCurrent_mAh>6400 ? aCurrent_mAh = 6400 : true; 	// limit max charge current to 6.400 A
	uint16_t realChargeCurrent = aCurrent_mAh/50;		// current are set in 50mAh steps
	busMaster.writeWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_IIN_HOST, realChargeCurrent<<8);
	return realChargeCurrent*50;
}

uint16_t BQ25700A::setChargeCurrent(uint16_t aCurrent_mAh) {
	aCurrent_mAh>8128 ? aCurrent_mAh = 8128 : true; 	// limit max charge current to 8.128 A
	uint16_t realChargeCurrent = aCurrent_mAh/64;		// the charger's charge current are in 65mAh steps
	busMaster.writeWordLSB(BQ25700A_DEFAULT_ADDR, BQ25700A_REG_CHARGECURRENT, realChargeCurrent<<6);
	return realChargeCurrent*64;
}


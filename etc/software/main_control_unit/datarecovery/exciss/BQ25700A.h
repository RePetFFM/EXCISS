/***********************************************************************
* FILENAME :        BQ2500A.h                      DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Library for charger ic BQ2500A from Texas Instruments.
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/

#ifndef _BQ25700A_H_
#define _BQ25700A_H_

#include <Arduino.h>

// #include "BusMaster.h"

/////////////////////////////
// define charger bq25700A //
/////////////////////////////

// define SMBus address for charger bq25700A
#define BQ25700A_DEFAULT_ADDR 0x09


// define register addresses for charger

#define BQ25700A_REG_CHARGE_OPTION_0		0x12		// Charg option 0 [r/w]

#define BQ25700A_REG_CHARGECURRENT			0x14		// sets the charg current for the battery [r/w]

#define BQ25700A_REG_MAX_CHARG_VOLTAGE		0x15		// maximum charge voltage [r/w]

#define BQ25700A_REG_CHARGE_OPTION_1		0x30		// Charg option 1 [r/w]

#define BQ25700A_REG_CHARGE_OPTION_2		0x31		// Charg option 2 [r/w]

#define BQ25700A_REG_CHARGE_OPTION_3		0x32		// Charg option 3 [r/w]

#define BQ25700A_REG_PROCHOT_OPTION_1		0x33		// PROCHOT option 0 [r/w]

#define BQ25700A_REG_PROCHOT_OPTION_2		0x34		// PROCHOT option 1 [r/w]

#define BQ25700A_REG_ADC_OPTION				0x35		// ADC option [r/w]

#define BQ25700A_REG_CHARGE_STATUS			0x20		// charge status [r]

#define BQ25700A_REG_PROCHOT_STATUS			0x21		// PROCHOT status [r]

#define BQ25700A_REG_IIN_LIMIT				0x22		// input current limit in use [r]

#define BQ25700A_REG_ADC_VBUS_PSYS			0x23		// ADC readout input and system voltage [r]

#define BQ25700A_REG_ADC_IBAT				0x24		// ADC readout battery charge and discharge current [r]

#define BQ25700A_REG_ADC_IIN_CMPIN			0x25		// ADC readout input current and CMPIN voltage [r]

#define BQ25700A_REG_ADC_VSYS_VBAT			0x26		// ADC readout system voltage and battery voltage [r]

#define BQ25700A_REG_OTG_VOLTAGE			0x3B		// not in use [r/w]

#define BQ25700A_REG_OTG_CURRENT			0x3C		// not in use [r/w]

#define BQ25700A_REG_INPUT_VOLTAGE			0x3D		// input voltage [r/w]

#define BQ25700A_REG_MIN_SYS_VOLTAGE		0x3E		// minimum system voltage [r/w]

#define BQ25700A_REG_IIN_HOST				0x3F		// input current limit set by host (microcontroller) [r/w]

#define BQ25700A_REG_MANUFACTURERID			0xFE		// Manufacturer ID - 0x0040H [r]

#define BQ25700A_REG_DEVICE_ADDRESS			0xFF		// Device address ID [r]




class BQ25700A {
public:

	// BQ25700A(uint8_t address);
	BQ25700A();

	void enableADC();

	bool init();

	void updateValues();

	uint16_t setImputCurrent(uint16_t aCurrent_mAh);

	uint16_t setChargeCurrent(uint16_t aCurrent_mAh);

	uint16_t chargeOption = 0x0;

	uint16_t currentLimit = 0;

	uint16_t chargeCurrent;

	uint16_t adcOption = 0x0;

	uint16_t adcVBUS = 0x0;

	uint16_t adcPSYS = 0x0;

	uint16_t adcIBATCharge = 0x0;

	uint16_t adcIBATDischarge = 0x0;

	uint16_t adcIIN = 0x0;

	uint16_t adcCMPIN = 0x0;

	uint16_t adcVSYS = 0x0;

	uint16_t adcVBAT = 0x0;

	
protected:

private:


	uint8_t _smbus_address;
	
};


#endif	// _BQ25700A_H_
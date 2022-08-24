/***********************************************************************
* FILENAME :        BQ40Z50.h                      DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Library for the smart batterypack manger BQ40Z50 from Texas Instruments.
* - Charger Texas Instruments bq25700A
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/

#ifndef _BQ40Z50_H_
#define _BQ40Z50_H_


// #include "BusMaster.h"


////////////////////////////////////////////
// define battery pack manager BQ40Z50-R2 //
////////////////////////////////////////////

// define SMBus address for battery pack manager BQ40Z50-R2
#define BQ40Z50_DEFAULT_ADDR	 				0x0B

#define BQ40Z50_MAC 						0x44


// BatteryMode()
// This read/write word function sets various battery operating mode options.
#define BQ40Z50_BATTERY_MODE				0x03	

// AtRateTimeToFull()
// This word read function returns the remaining time-to-fully charge the battery stack.
#define BQ40Z50_TIME_TO_FULL				0x05	

// AtRateTimeToEmpty()
// This word read function returns the remaining time-to-fully discharge the battery stack.
#define BQ40Z50_TIME_TO_EMPTY				0x06	

// Temperature()
// This read-word function returns the temperature in units 0.1°K.
#define BQ40Z50_TEMPERATURE				0x08	

// Voltage()
// This read-word function returns the sum of the measured cell voltages.
#define BQ40Z50_VOLTAGE					0x09	

// Current()
// This read-word function returns the measured current from the coulomb counter. 
// If the input to the device exceeds the maximum value, the value is clamped at the maximum and does not roll over.]
#define BQ40Z50_CURRENT					0x0A

// AverageCurrent()
#define BQ40Z50_AVERAGE_CURRENT			0x0B

// MaxError()
// This read-word function returns the expected margin of error, in %, in the state-of-charge calculation with a range of 1 to 100%.
#define BQ40Z50_MAX_ERROR					0x0C

// RelativeStateOfCharge()
// This read-word function returns the predicted remaining battery capacity as a percentage of FullChargeCapacity().
#define BQ40Z50_RELATIVE_CHARGE			0x0D

// AbsoluteStateOfCharge()
// This read-word function returns the predicted remaining battery capacity as a percentage.
#define BQ40Z50_ABSOLUTE_CHARGE			0x0E

// RemainingCapacity()
// This read-word function returns the predicted remaining battery capacity.
// NOTE: 
// If BatteryMode()[CAPM] = 0, then the data reports in mAh.
// If BatteryMode()[CAPM] = 1, then the data reports in 10 mWh.
#define BQ40Z50_REMAINING_CAPACITY			0x0F

// FullChargeCapacity()
// This read-word function returns the predicted battery capacity when fully charged. The value returned will not be updated during charging.
// NOTE: 
// If BatteryMode()[CAPM] = 0, then the data reports in mAh.
// If BatteryMode()[CAPM] = 1, then the data reports in 10 mWh.
#define BQ40Z50_FULL_CHARGE_CAPACITY		0x10

// RunTimeToEmpty()
// This read-word function returns the predicted remaining battery capacity based on the present rate of discharge.
#define BQ40Z50_RUNTIME_TO_EMPTY			0x11

// BatteryStatus()
// This read-word function returns various battery status information.
#define BQ40Z50_BATTERY_STATUS				0x16
// OCA (Bit 15): Overcharged Alarm 1 = Detected
//   0 = Not Detected
// TCA (Bit 14): Terminate Charge Alarm 1 = Detected
//   0 = Not Detected RSVD (Bit 13): Undefined
// OTA (Bit 12): Overtemperature Alarm 1 = Detected
//   0 = Not Detected
// TDA (Bit 11): Terminate Discharge Alarm 1 = Detected
//   0 = Not Detected RSVD (Bit 10): Undefined
// RCA (Bit 9): Remaining Capacity Alarm
//   1 = RemainingCapacity() < RemainingCapacityAlarm() when in DISCHARGE or RELAX mode 0 = RemainingCapacity() ≥ RemainingCapacityAlarm()
// RTA (Bit 8): Remaining Time Alarm
//   1 = AverageTimeToEmpty() < RemainingTimeAlarm() or 0 = AverageTimeToEmpty() ≥ RemainingTimeAlarm()
// INIT (Bit 7): Initialization
//   1 = Gauge initialization is complete. 0 = Initialization is in progress.
// DSG (Bit 6): Discharging or Relax
//   1 = Battery is in DISCHARGE or RELAX mode. 0 = Battery is in CHARGE mode.
// FC (Bit 5): Fully Charged
//   1 = Battery fully charged when GaugingStatus()[FC] = 1, 0 = Battery not fully charged
// FD (Bit 4): Fully Discharged
//   1 = Battery fully depleted, 0 = Battery not depleted
// EC3,EC2,EC1,EC0 (Bits 3–0): Error Code 0x0= OK
//   0x1 = Busy, 0x2 = Reserved Command, 0x3 = Unsupported Command, 0x4 = AccessDenied, 0x5 = Overflow/Underflow

// CycleCount()
// This read-word function returns the number of discharge cycles the battery has experienced. 
// The default value is stored in the data flash value Cycle Count, which is updated in runtime.
#define BQ40Z50_CYCLE_COUNT				0x17

// CellVoltage4()
// This read-word function returns the Cell 4 voltage.
#define BQ40Z50_CELL_4_VOLTAGE				0x3C

// CellVoltage3()
// This read-word function returns the Cell 3 voltage.
#define BQ40Z50_CELL_3_VOLTAGE				0x3D

// CellVoltage2()
// This read-word function returns the Cell 2 voltage.
#define BQ40Z50_CELL_2_VOLTAGE				0x3E

// CellVoltage1()
// This read-word function returns the Cell 1 voltage.
#define BQ40Z50_CELL_1_VOLTAGE				0x3F


class BQ40Z50 {

public:
	BQ40Z50 ();

	bool init();

	void updateValues();

	uint16_t batteryMode = 0x0;

	uint16_t maxError = 0x0;

	uint16_t voltage = 0x0;

	int16_t current = 0x0;

	uint8_t batteryChemID[4];

	uint16_t remainingCurrent = 0x0;
	
	uint16_t fullChargCapacity = 0x0;
	
	uint16_t minToFull = 0x0;
	
	uint16_t minToEmpty = 0x0;

	uint16_t minRuntime = 0x0;
	
	float temperature = 0.0;

	uint16_t chargeRelative = 0x0;

	uint16_t chargeAbsolute = 0x0;

	uint16_t batteryStatus = 0x0;
	
	uint16_t cycleCount = 0x0;
	
	uint16_t cell1voltage = 0x0;
	
	uint16_t cell2voltage = 0x0;
	
	uint16_t cell3voltage = 0x0;

	uint16_t cell4voltage = 0x0;


	

	

protected:

private:

	uint16_t readWordManufacturer(uint8_t aI2CDeviceAddress, uint8_t aFunction, uint16_t aManufacturerFunction);


};

#endif	// _BQ40Z50_H_
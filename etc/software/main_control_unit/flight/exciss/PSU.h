/***********************************************************************
* FILENAME :        PowerManager.h                 DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Powermanagment library.
* Follwoing hardware components are controlled by this library:
* - Charger Texas Instruments bq25700A
* - Battery pack manager and gauge from Texas Instruments BQ40Z50-R2
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/




#ifndef _PSU_H_
#define _PSU_H_

#include <Arduino.h>

#include "config_atmega_port_mapping.h"

#include "BusMaster.h"

#include "BQ25700A.h"

#include "BQ40Z50.h"

#include "COM.h"


/////////////////////////////
// define charger bq25700A //
/////////////////////////////

// define SMBus address for charger bq25700A
#define CHARGER_ADDR 0x09


// define register addresses for charger

#define CHARGER_REG_CHARGE_OPTION_0		0x12		// Charg option 0 [r/w]

#define CHARGER_REG_CHARGECURRENT		0x14		// sets the charg current for the battery [r/w]

#define CHARGER_REG_MAX_CHARG_VOLTAGE	0x15		// maximum charge voltage [r/w]

#define CHARGER_REG_CHARGE_OPTION_1		0x30		// Charg option 1 [r/w]

#define CHARGER_REG_CHARGE_OPTION_2		0x31		// Charg option 2 [r/w]

#define CHARGER_REG_CHARGE_OPTION_3		0x32		// Charg option 3 [r/w]

#define CHARGER_REG_PROCHOT_OPTION_1	0x33		// PROCHOT option 0 [r/w]

#define CHARGER_REG_PROCHOT_OPTION_2	0x34		// PROCHOT option 1 [r/w]

#define CHARGER_REG_ADC_OPTION			0x35		// ADC option [r/w]

#define CHARGER_REG_CHARGE_STATUS		0x20		// charge status [r]

#define CHARGER_REG_PROCHOT_STATUS		0x21		// PROCHOT status [r]

#define CHARGER_REG_IIN_LIMIT			0x22		// input current limit in use [r]

#define CHARGER_REG_ADC_VBUS_PSYS		0x23		// ADC readout input and system voltage [r]

#define CHARGER_REG_ADC_IBAT			0x24		// ADC readout battery charge and discharge current [r]

#define CHARGER_REG_ADC_IIN_CMPIN		0x25		// ADC readout input current and CMPIN voltage [r]

#define CHARGER_REG_ADC_VSYS_VBAT		0x26		// ADC readout system voltage and battery voltage [r]

#define CHARGER_REG_OTG_VOLTAGE			0x3B		// not in use [r/w]

#define CHARGER_REG_OTG_CURRENT			0x3C		// not in use [r/w]

#define CHARGER_REG_INPUT_VOLTAGE		0x3D		// input voltage [r/w]

#define CHARGER_REG_MIN_SYS_VOLTAGE		0x3E		// minimum system voltage [r/w]

#define CHARGER_REG_IIN_HOST			0x3F		// input current limit set by host (microcontroller) [r/w]

#define CHARGER_REG_MANUFACTURERID		0xFE		// Manufacturer ID - 0x0040H [r]

#define CHARGER_REG_DEVICE_ADDRESS		0xFF		// Device address ID [r]





////////////////////////////////////////////
// define battery pack manager BQ40Z50-R2 //
////////////////////////////////////////////

// define SMBus address for battery pack manager BQ40Z50-R2
#define BATTERY_PACK_MANAGER_ADDR 				0x0B

#define BATTERY_PACK_MAC 						0x00

// AtRateTimeToFull()
// This word read function returns the remaining time-to-fully charge the battery stack.
#define BATTERY_PACK_TIME_TO_FULL				0x05	

// AtRateTimeToEmpty()
// This word read function returns the remaining time-to-fully discharge the battery stack.
#define BATTERY_PACK_TIME_TO_EMPTY				0x06	

// Temperature()
// This read-word function returns the temperature in units 0.1°K.
#define BATTERY_PACK_TEMPERATURE				0x08	

// Voltage()
// This read-word function returns the sum of the measured cell voltages.
#define BATTERY_PACK_VOLTAGE					0x09	

// Current()
// This read-word function returns the measured current from the coulomb counter. 
// If the input to the device exceeds the maximum value, the value is clamped at the maximum and does not roll over.]
#define BATTERY_PACK_CURRENT					0x0A

// AverageCurrent()
#define BATTERY_PACK_AVERAGE_CURRENT			0x0B

// MaxError()
// This read-word function returns the expected margin of error, in %, in the state-of-charge calculation with a range of 1 to 100%.
#define BATTERY_PACK_MAX_ERROR					0x0C

// RelativeStateOfCharge()
// This read-word function returns the predicted remaining battery capacity as a percentage of FullChargeCapacity().
#define BATTERY_PACK_RELATIVE_CHARGE			0x0D

// AbsoluteStateOfCharge()
// This read-word function returns the predicted remaining battery capacity as a percentage.
#define BATTERY_PACK_ABSOLUTE_CHARGE			0x0E

// RemainingCapacity()
// This read-word function returns the predicted remaining battery capacity.
// NOTE: 
// If BatteryMode()[CAPM] = 0, then the data reports in mAh.
// If BatteryMode()[CAPM] = 1, then the data reports in 10 mWh.
#define BATTERY_PACK_REMAINING_CAPACITY			0x0F

// FullChargeCapacity()
// This read-word function returns the predicted battery capacity when fully charged. The value returned will not be updated during charging.
// NOTE: 
// If BatteryMode()[CAPM] = 0, then the data reports in mAh.
// If BatteryMode()[CAPM] = 1, then the data reports in 10 mWh.
#define BATTERY_PACK_FULL_CHARGE_CAPACITY		0x10

// RunTimeToEmpty()
// This read-word function returns the predicted remaining battery capacity based on the present rate of discharge.
#define BATTERY_PACK_RUNTIME_TO_EMPTY			0x11

// BatteryStatus()
// This read-word function returns various battery status information.
#define BATTERY_PACK_BATTERY_STATUS				0x16
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
#define BATTERY_PACK_CYCLE_COUNT				0x17

// CellVoltage4()
// This read-word function returns the Cell 4 voltage.
#define BATTERY_PACK_CELL_4_VOLTAGE				0x3C

// CellVoltage3()
// This read-word function returns the Cell 3 voltage.
#define BATTERY_PACK_CELL_3_VOLTAGE				0x3D

// CellVoltage2()
// This read-word function returns the Cell 2 voltage.
#define BATTERY_PACK_CELL_2_VOLTAGE				0x3E

// CellVoltage1()
// This read-word function returns the Cell 1 voltage.
#define BATTERY_PACK_CELL_1_VOLTAGE				0x3F


// extern COM com;


void PSU__init();

void PSU__charge_current(int aCurrent);

void PSU__max_input_current(int aCurrent);

void PSU__mode_normal();

void PSU__mode_mcu_only();

uint8_t PSU__charg_ok();

uint8_t PSU__battery_level();

void PSU__logoutCharger();

void PSU__logoutGauge();








#endif	// _PSU_H_


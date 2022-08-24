/***********************************************************************
* FILENAME :        PowerManager.cpp               DESIGN REF: EXCISS_v4
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

#include <Arduino.h>

#include "PSU.h" 

#include "exciss.h" 

#include "BQ25700A.h"

#include "BQ40Z50.h"

#include "config_atmega_port_mapping.h"

// BusMaster busMaster;

// BQ25700A charger(BQ25700A_DEFAULT_ADDR);

// BQ40Z50 gauge(BQ40Z50_DEFAULT_ADDR);

// BQ25700A PSU__PSU__charger;

// BQ40Z50 PSU___gauge;
uint16_t PSU__last_battery_status;

BQ25700A PSU__charger;

BQ40Z50 PSU__gauge;

void PSU__init() 
{
  pinMode( DIN_PSU_CHRG_OK , INPUT );

  bool chargerOk = PSU__charger.init();

  bool gaugeOk = PSU__gauge.init();
}

void PSU__charge_current(int aCurrent) 
{
  PSU__charger.setChargeCurrent(aCurrent);
}

void PSU__max_input_current(int aCurrent) 
{
  PSU__charger.setImputCurrent(aCurrent);
}

void PSU__mode_normal() 
{
  PSU__max_input_current(850);
  PSU__charge_current(350);
}

void PSU__mode_mcu_only() 
{
  PSU__max_input_current(200);
  PSU__charge_current(0);
}


uint8_t PSU__charg_ok() 
{
  uint8_t charg_ok = 0xAA;

  digitalRead( DIN_PSU_CHRG_OK ) == HIGH ? charg_ok = 0xAA : charg_ok = 0x55;
  
  return charg_ok;
}


uint8_t PSU__battery_level()
{
  // returns charge level in percentage
  
  PSU__gauge.updateValues();
  
  return PSU__gauge.chargeAbsolute;
}


void PSU__logoutCharger() 
{
  
  
  delay(1);

  com.log("[ POWER ][ PSU ][ charger ]");
  com.newline();

  PSU__charger.updateValues();

  delay(1);

  com.log("adcVBUS = ");
  com.log(PSU__charger.adcVBUS);
  com.newline();

  com.log("adcPSYS = ");
  com.log(PSU__charger.adcPSYS);
  com.newline();

  com.log("adcIBATCharge = ");
  com.log(PSU__charger.adcIBATCharge);
  com.newline();

  com.log("adcIBATDischarge = ");
  com.log(PSU__charger.adcIBATDischarge);
  com.newline();

  com.log("adcIIN = ");
  com.log(PSU__charger.adcIIN);
  com.newline();

  com.log("adcCMPIN = ");
  com.log(PSU__charger.adcCMPIN);
  com.newline();

  com.log("adcVSYS = ");
  com.log(PSU__charger.adcVSYS);
  com.newline();

  com.log("adcVBAT = ");
  com.log(PSU__charger.adcVBAT);
  com.newline();

  com.log("currentLimit = ");
  com.log(PSU__charger.currentLimit);
  com.newline();
  
  com.log("adcOption = ");
  com.logBIN(PSU__charger.adcOption);
  com.newline();

  com.log("chargeOption = ");
  com.logBIN(PSU__charger.chargeOption);
  com.newline();
  
  com.log("chargeCurrent = ");
  com.logHEX(PSU__charger.chargeCurrent);
  com.newline();
  

}

void PSU__logoutGauge() {
  // gauge readout
  com.log("[ POWER ][ PSU ][ gauge ]");
  com.newline();
  
  PSU__gauge.updateValues();

  
  com.log("batteryMode = ");
  com.logBIN(PSU__gauge.batteryMode);
  com.newline();

  com.log("maxError = ");
  com.log(PSU__gauge.maxError);
  com.newline();

  com.log("voltage = ");
  com.log(PSU__gauge.voltage);
  com.newline();

  com.log("current = ");
  com.log(PSU__gauge.current);
  com.newline();

  com.log("batteryChemID = ");
  com.log("waiting for implementation");
  // com.log(PSU__gauge.batteryChemID);
  com.newline();

  

  com.log("remainingCurrent = ");
  com.log(PSU__gauge.remainingCurrent);
  com.newline();

  com.log("fullChargCapacity = "); 
  com.log(PSU__gauge.fullChargCapacity);
  com.newline();

  com.log("minToFull = "); 
  com.log(PSU__gauge.minToFull);
  com.newline();

  com.log("minToEmpty = "); 
  com.log(PSU__gauge.minToEmpty);
  com.newline();

  com.log("minRuntime = ");
  com.log(PSU__gauge.minRuntime);
  com.newline();

  com.log("temperature = "); 
  com.logFloat(PSU__gauge.temperature);
  com.newline();

  com.log("chargeRelative = ");
  com.log(PSU__gauge.chargeRelative);
  com.newline();

  com.log("chargeAbsolute = ");
  com.log(PSU__gauge.chargeAbsolute);
  com.newline();

  com.log("batteryStatus = ");
  com.logBIN(PSU__gauge.batteryStatus); // see github issue https://github.com/exciss-space/EXCISS/issues/3
  com.newline();

  com.log("cycleCount = "); 
  com.log(PSU__gauge.cycleCount);
  com.newline();

  com.log("cell1voltage = "); 
  com.log(PSU__gauge.cell1voltage);
  com.newline();

  com.log("cell2voltage = "); 
  com.log(PSU__gauge.cell2voltage);
  com.newline();
  
  com.log("cell3voltage = "); 
  com.log(PSU__gauge.cell3voltage);
  com.newline();

  com.log("cell4voltage = ");
  com.log(PSU__gauge.cell4voltage);  
  com.newline();
}
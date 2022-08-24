/***********************************************************************
*
* DESCRIPTION :
* AUTHOR :    Shintaro Fujita
*
**/


#ifndef _TEMPSENS_H_
#define _TEMPSENS_H_

#include "DallasTemperature.h"

#define DS2482_COMMAND_SINGLEBIT  0x87

struct TEMPSENS {
	DeviceAddress address;
	float temperature;
};

void TEMPSENS__init();

void TEMPSENS__read_address_from_eeprom(uint16_t aEepromStart, DeviceAddress aAddress);

void TEMPSENS__printAddress(uint8_t index);

void TEMPSENS__logout_all_temperatur_sensors();

void TEMPSENS__logout_temperatur_sensor(uint8_t aId);


#endif
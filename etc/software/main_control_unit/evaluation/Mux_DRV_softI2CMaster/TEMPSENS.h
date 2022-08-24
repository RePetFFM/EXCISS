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

void TEMPSENS__update_queed_temeperatur_sensor();

void TEMPSENS__logout_all_temperatur_sensors();

void TEMPSENS__logout_temperatur_sensor(uint8_t aId);

#endif
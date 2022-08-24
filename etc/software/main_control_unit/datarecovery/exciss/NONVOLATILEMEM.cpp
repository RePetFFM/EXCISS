/***********************************************************************
*
* DESCRIPTION : eeprom wrapper library
* AUTHOR :    Shintaro Fujita
*
* MORE : https://github.com/exciss-space/EXCISS/wiki/MCU-EEPROM-data-structure
**/



#include "NONVOLATILEMEM.h"
#include "exciss.h"
#include <EEPROM.h>

void NONVOLATILEMEM__init() {

}

uint8_t NONVOLATILEMEM__read( int aAddr ) {
	return EEPROM.read( aAddr );
}

void NONVOLATILEMEM__write( int aAddr, uint8_t aData ) {
	return EEPROM.update( aAddr , aData );
}

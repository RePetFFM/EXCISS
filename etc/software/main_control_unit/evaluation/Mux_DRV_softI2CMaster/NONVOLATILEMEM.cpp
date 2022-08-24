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

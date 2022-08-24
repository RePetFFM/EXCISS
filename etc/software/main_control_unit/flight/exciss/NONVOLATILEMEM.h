/***********************************************************************
*
* DESCRIPTION : eeprom wrapper library
* AUTHOR :    Shintaro Fujita
*
* MORE : https://github.com/exciss-space/EXCISS/wiki/MCU-EEPROM-data-structure
**/


#include "exciss.h"

#ifndef _NONVOLATILEMEM_H_
#define _NONVOLATILEMEM_H_


void NONVOLATILEMEM__init();

uint8_t NONVOLATILEMEM__read( int aAddr );

void NONVOLATILEMEM__write( int aAddr, uint8_t aData );


#endif
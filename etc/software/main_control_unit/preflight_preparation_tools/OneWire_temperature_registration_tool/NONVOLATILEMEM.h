#ifndef _NONVOLATILEMEM_H_
#define _NONVOLATILEMEM_H_


void NONVOLATILEMEM__init();

uint8_t NONVOLATILEMEM__read( int aAddr );

void NONVOLATILEMEM__write( int aAddr, uint8_t aData );


#endif
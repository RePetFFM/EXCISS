/***********************************************************************
* DESCRIPTION :
*       main arduino code for EXCISS version 4, PCB revision 1.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    18 Feb 2018
*
**/



#ifndef _COM_H_
#define _COM_H_

#include <Arduino.h>

#include "config.h"
#include "config_atmega_port_mapping.h"
#include "config_statemachine_definitions.h"



class COM {
public:
	COM();

	void loop();

	void begin(int baud);

	void newline();

	void log(char* aMsg);

	void log(char * aMsg, bool aOnlyDebug);

	void log(int aMsg);

	void logFloat(float aMsg);

	void log(String aMsg);

	void log_uint32_t(uint32_t aMsg);

	void logBIN(int aMsg);

	void logBIN(uint16_t aMsg);

	void logHEX(int aMsg);


private:
	void parserSCU();

	void parserDEBUG();

	void ParserRead(char * buf,uint8_t cnt);

	void ParserWrite(char * buf,uint8_t cnt);

	void ParserExecute(char * buf,uint8_t cnt);

	char ECC(byte repeats, char * buf);

	char readSingleHex(char c);

	char *intToHex(int a_i);

// void printLnString(String msg);

};

#endif // _COM_H_
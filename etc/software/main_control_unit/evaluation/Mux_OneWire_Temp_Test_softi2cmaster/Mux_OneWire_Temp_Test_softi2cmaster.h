/***********************************************************************
* FILENAME :        exciss.h                         DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Power managment testing and avaluation software.
* Follwoing hardware components are controlled by this library:
* - Charger Texas Instruments bq25700A
* - Battery pack manager and gauge from Texas Instruments BQ40Z50-R2
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/

#ifndef _EXCISS_H_
#define _EXCISS_H_

#include "BusMaster.h"
// declerations for services
extern BusMaster busMaster;

#endif	// _EXCISS_H_
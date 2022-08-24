/***********************************************************************
* DESCRIPTION :
*       Service for usb mass storage operations
*
* 
* AUTHOR :    Shintaro Fujita
*
**/

#ifndef _USBMASSSTORAGE_H
#define _USBMASSSTORAGE_H

#include "exciss.h"


void USBSTORAGE__init();

void USBSTORAGE__x_to_iss();

void USBSTORAGE__x_to_scu();

void USBSTORAGE__x_on();

void USBSTORAGE__x_off();

void USBSTORAGE__a_on();

void USBSTORAGE__a_off();

void USBSTORAGE__b_on();

void USBSTORAGE__b_off();

void USBSTORAGE__hub_on();

void USBSTORAGE__hub_off();

void USBSTORAGE__hub_reset();



#endif // end _USBMASSSTORAGE_H
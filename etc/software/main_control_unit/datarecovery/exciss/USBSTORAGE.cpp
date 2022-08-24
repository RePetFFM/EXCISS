/***********************************************************************
* DESCRIPTION :
*       Service for usb masstorage operations
*
* 
* AUTHOR :    Shintaro Fujita
*
**/


#include "USBSTORAGE.h"


uint8_t _usb_x_connected; // 0 = iss, 1 = scu

uint8_t _usb_x_enabled;

uint8_t _usb_a_enabled;

uint8_t _usb_b_enabled;

uint8_t _usb_hub_enabled;



void USBSTORAGE__init() {
    pinMode( DOUT_USB_X_TOGGLE , OUTPUT );

    pinMode( DOUT_USB_X_PWR , OUTPUT );
    
    pinMode( DOUT_USB_A_PWR , OUTPUT );
    
    pinMode( DOUT_USB_B_PWR , OUTPUT );
    
    pinMode( DOUT_USB_X_TOGGLE , OUTPUT);
    
    pinMode( DOUT_USBHUB_PWR , OUTPUT );
    
    USBSTORAGE__x_off();
    USBSTORAGE__a_off();
    USBSTORAGE__b_off();
    USBSTORAGE__hub_off();

    pinMode( DOUT_USBHUB_PORT_1 , OUTPUT );
    pinMode( DOUT_USBHUB_PORT_2 , OUTPUT );

    pinMode( DOUT_USBHUB_RESET , STATE_USBHUB_NOT_RESET );
    digitalWrite( DOUT_USBHUB_RESET , LOW);
}

void USBSTORAGE__x_to_iss() {
  digitalWrite( DOUT_USB_X_TOGGLE , STATE_USB_X_TO_ISS );

  _usb_x_connected = 0;
}

void USBSTORAGE__x_to_scu() {
    digitalWrite( DOUT_USB_X_TOGGLE , STATE_USB_X_TO_SCU );
    
    _usb_x_connected = 1;
}

void USBSTORAGE__x_on() {
    digitalWrite( DOUT_USB_X_PWR , STATE_USB_ON );

    _usb_x_enabled = 1;
}

void USBSTORAGE__x_off() {
    digitalWrite( DOUT_USB_X_PWR , STATE_USB_OFF );

    _usb_x_enabled = 0;
}

void USBSTORAGE__a_on() {
    digitalWrite( DOUT_USB_A_PWR , STATE_USB_ON );

    _usb_a_enabled = 1;
}

void USBSTORAGE__a_off() {
    digitalWrite( DOUT_USB_A_PWR , STATE_USB_OFF );

    _usb_a_enabled = 0;
}

void USBSTORAGE__b_on() {
    digitalWrite( DOUT_USB_B_PWR , STATE_USB_ON );

    _usb_b_enabled = 1;
}

void USBSTORAGE__b_off() {
    digitalWrite( DOUT_USB_B_PWR , STATE_USB_OFF );

    _usb_b_enabled = 0;
}

void USBSTORAGE__hub_on() {
    if(_usb_hub_enabled==0) {

        USBSTORAGE__hub_reset();

        digitalWrite( DOUT_USBHUB_PWR , STATE_USBHUB_PWR_ON );  

        _usb_hub_enabled = 1;
    }
    
}

void USBSTORAGE__hub_off() {
    if(_usb_a_enabled==0 && _usb_b_enabled==0) {

        digitalWrite( DOUT_USBHUB_PWR , STATE_USBHUB_PWR_OFF ); 

        _usb_hub_enabled = 0;
    }
}

void USBSTORAGE__hub_reset() {
    pinMode( DOUT_USBHUB_RESET , STATE_USBHUB_RESET );
    digitalWrite( DOUT_USBHUB_RESET , LOW);
    delay(1);
    pinMode( DOUT_USBHUB_RESET , STATE_USBHUB_NOT_RESET );
    digitalWrite( DOUT_USBHUB_RESET , LOW);
}


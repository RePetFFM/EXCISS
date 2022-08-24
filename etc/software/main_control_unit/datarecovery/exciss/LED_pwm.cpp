/***********************************************************************
* DESCRIPTION :
*       library for led brightness control using pwm
*
* 
* AUTHOR :    Shintaro Fujita
*
**/


#include "Arduino.h"
#include "LED_pwm.h"


LED_pwm::LED_pwm(int apin) {
  pinMode( apin , OUTPUT);
  off();
  _pin = apin;
}

void LED_pwm::timer_divisor(byte adiv) {
  // TCCR5B = TCCR5B & 0b11111000 | 0x01;
  TCCR5B = TCCR5B & 0b11111000 | adiv; // 0x01 = 31250Hz/1, 0x02 = 31250Hz/8  
}


void LED_pwm::off() {
  on( 0 );
}

void LED_pwm::on(int apwm) {
  analogWrite( _pin , apwm);
}

/***********************************************************************
* DESCRIPTION :
*       library for led brightness control using pwm
*
* 
* AUTHOR :    Shintaro Fujita
*
**/


#ifndef LED_pwm_h
#define LED_pwm_h

#include "Arduino.h"


class LED_pwm {
  public:
    LED_pwm(int apin);

    void timer_divisor(byte adiv);

    void off();

    void on(int apwm);

  private:
    int _pin;
};

#endif
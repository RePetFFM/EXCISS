/***********************************************************************
* DESCRIPTION :
*
* 
* AUTHOR :    Shintaro Fujita
*
**/

#include "LIGHTS.h"
#include "exciss.h"

LED_pwm led_front( PWMOUT_FRONTLIGHT_CONTROL );

LED_pwm led_back( PWMOUT_BACKLIGHT_CONTROL );

void LIGHTS__init() {
  led_front.timer_divisor(0x01);
  led_front.on(0);
  led_back.on(0);
}

void LIGHTS__front_led_pwm(int a_pwm) {
  com.log("[ LIGHTS ]");
  com.newline();
  com.log("LED_front = ");
  com.log( a_pwm );
  com.newline();

  led_front.on(a_pwm);
}

void LIGHTS__back_led_pwm(int a_pwm) {
  com.log("[ LIGHTS ]");
  com.newline();
  com.log("LED_back = ");
  com.log( a_pwm );
  com.newline();

  led_back.on(a_pwm);
}

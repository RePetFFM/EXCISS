/***********************************************************************
* DESCRIPTION :
*       main arduino code for EXCISS version 4, PCB revision 1.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    18 Feb 2018
*
**/

#include <avr/wdt.h>

#include "config.h"
#include "portmap.h"
#include "statemachine_definitions.h"

#include <Wire.h>
#include "hvu.h"


#include "PCA9546.h"
#include "RTClib.h"


#include "Adafruit_DRV2605.h"
// #include <DallasTemperature.h>


// RTC_DS3231 rtc;

uint8_t SM_SCU__state_operation_mode = SM_SCU__OPERATION_MODE_NORMAL;

uint8_t SM_MCU__state = SM_MCU__STATE_INIT;




unsigned long SCU__keepalive_timer;
unsigned long SCU__wakeup_timer;


RTC_DS3231 rtc;

Adafruit_DRV2605 drv;

uint8_t effect = 1;


void setup() {
    // wdt_enable(WDTO_8S); // set watchdog interval to 8 seconds

    // analogWriteResolution(16);

    pinMode( A8 , INPUT );
    pinMode( A9 , INPUT );
    pinMode( A10 , INPUT );
    pinMode( A11 , INPUT );
    pinMode( A12 , INPUT );
    pinMode( A13 , INPUT );
    pinMode( A14 , OUTPUT );

    digitalWrite(A14, HIGH);

    // init serials
    Serial.begin( UART_BAUTRATE );
    Serial.println("serial debug rdy");


    Serial.println("test");

    delay(500);

    Serial.println("test1");

    // wdt_reset();
    
    Serial.println("test2");

    // delay(10);

    
    // wdt_reset(); // reset watchdog timer
}


void loop() {
    if(SM_MCU__state == SM_MCU__STATE_INIT) {
        // Serial.println(CORE__init_state,HEX);
        MCU__statemachine_init();
    } 

    if(SM_MCU__state == SM_MCU__STATE_NORMAL) {
        SERIAL__Parser();

        HVU__statemachine();
    }

}

void MCU__statemachine_init() {
    MCU__init_pins();    

    SM_MCU__state = SM_MCU__STATE_NORMAL;
}


void MCU__init_pins() {
    TCCR5B = TCCR5B & 0b11111000 | 0x01; // 0x01 = 31250Hz/1, 0x02 = 31250Hz/8


    pinMode( DOUT_5V_ENABLE , OUTPUT );
    digitalWrite( DOUT_5V_ENABLE , LOW );

    pinMode( DOUT_12V_ENABLE , OUTPUT );
    digitalWrite( DOUT_12V_ENABLE , LOW );

    pinMode( DOUT_SCU_PWR , OUTPUT );
    digitalWrite( DOUT_SCU_PWR , HIGH );

    pinMode( DOUT_SCU_SD_TOGGLE , OUTPUT );

    pinMode( DOUT_SCU_SHUTDOWN_REQUEST , OUTPUT );
    digitalWrite(DOUT_SCU_SHUTDOWN_REQUEST, LOW);

    pinMode( DOUT_USB_X_PWR , OUTPUT );
    digitalWrite( DOUT_USB_X_PWR , HIGH );

    pinMode( DOUT_USB_A_PWR , OUTPUT );
    digitalWrite( DOUT_USB_A_PWR , HIGH );

    pinMode( DOUT_USB_B_PWR , OUTPUT );
    digitalWrite( DOUT_USB_B_PWR , HIGH );

    pinMode( DOUT_USB_X_TOGGLE , OUTPUT);
    digitalWrite( DOUT_USB_X_TOGGLE, USB_X_TO_SCU);


    pinMode( DOUT_USBHUB_PWR , OUTPUT );
    digitalWrite( DOUT_USBHUB_PWR , HIGH );

    pinMode( DOUT_USBHUB_PORT_1 , OUTPUT );
    pinMode( DOUT_USBHUB_PORT_2 , OUTPUT );

    pinMode( DOUT_USBHUB_RESET , USBHUB_NOT_RESET );
    digitalWrite( DOUT_USBHUB_RESET , LOW); 


    pinMode( PWMOUT_FRONTLIGHT_CONTROL , OUTPUT);
    analogWrite( PWMOUT_FRONTLIGHT_CONTROL , 0);

    pinMode( PWMOUT_BACKLIGHT_CONTROL , OUTPUT);
    analogWrite( PWMOUT_BACKLIGHT_CONTROL , 0);


    pinMode( DOUT_MOT_ENABLE , OUTPUT );
    digitalWrite( DOUT_MOT_ENABLE , LOW); 
    


    pinMode( DOUT_I2CMUX_RST , OUTPUT );
    digitalWrite( DOUT_I2CMUX_RST , HIGH); 



    digitalWrite( DOUT_5V_ENABLE , HIGH );


    digitalWrite( DOUT_MOT_ENABLE , HIGH ); 

    delay(100);

    // scan();

    PCA9546 pca(0x70);


    bool bSuccess = pca.selectChannel(0x01); // no channel
    Serial.print("No channel: ");
    Serial.println(bSuccess);
    Serial.println("");

    uint8_t temp = pca.i2c_read();
    Serial.print("i2c_read() : ");
    Serial.println(temp);

    scan();


    bSuccess = pca.selectChannel(0x02); // no channel
    Serial.print("No channel: ");
    Serial.println(bSuccess);
    Serial.println("");

    temp = pca.i2c_read();
    Serial.print("i2c_read() : ");
    Serial.println(temp);

    scan();


    bSuccess = pca.selectChannel(0x04); // no channel
    Serial.print("No channel: ");
    Serial.println(bSuccess);
    Serial.println("");

    temp = pca.i2c_read();
    Serial.print("i2c_read() : ");
    Serial.println(temp);

    scan();


    bSuccess = pca.selectChannel(0x08); // no channel
    Serial.print("No channel: ");
    Serial.println(bSuccess);
    Serial.println("");

    temp = pca.i2c_read();
    Serial.print("i2c_read() : ");
    Serial.println(temp);

    scan();



    bSuccess = pca.selectChannel(0x01); // no channel
    Serial.print("No channel: ");
    Serial.println(bSuccess);
    Serial.println("");

    temp = pca.i2c_read();
    Serial.print("i2c_read() : ");
    Serial.println(temp);

    scan();


    drv.begin();
  
    drv.useERM();
    drv.selectLibrary(1);

    // I2C trigger by sending 'go' command 
    // default, internal trigger when sending GO command
    drv.setMode(DRV2605_MODE_INTTRIG); 
    

    drv.setWaveform(0, effect);  // play effect 
    drv.setWaveform(1, 0);       // end waveform

    // play the effect!
    drv.go();
  
  /*

  bSuccess = pca.selectChannel(1); // no channel
  Serial.print("No channel: ");
  Serial.println(bSuccess);
  Serial.println("");

  temp = pca.i2c_read();
  Serial.print("i2c_read() : ");
  Serial.println(temp);

  scan();


  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  //////////////////////////////////////////
  // PORT 2 = 0x1
  {
    bSuccess = pca.selectChannel(0x8);
    Serial.print("Channel ");
    Serial.print(0x4);
    Serial.print(": ");
    Serial.println(bSuccess);
  
    temp = pca.i2c_read();
    Serial.print("i2c_read() : ");
    Serial.println(temp);
  
    
    checkRTC();
    
    
    scan();
  }
  */

}


void checkRTC()
{
      
    //RTC_DS3231 rtc;
//    if (rtc.begin()) 
//    {
      //Serial.println("RTC found");
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      DateTime now = rtc.now();
      
      Serial.print(now.year(), DEC);
      Serial.print('-');
      Serial.print(now.month(), DEC);
      Serial.print('-');
      Serial.print(now.day(), DEC);
      Serial.print(' ');
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();
//    }
//    else
//    {
//      Serial.println("RTC NOT found");
//    }
  
}



void scan()
{
    byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: PowerLED

void POWERLED_frontlight(int pwm_duty_circle) {
    analogWrite(PWMOUT_FRONTLIGHT_CONTROL, pwm_duty_circle);
}

void POWERLED_backlight(int pwm_duty_circle) {
    analogWrite(PWMOUT_BACKLIGHT_CONTROL, pwm_duty_circle);
}

// end: PowerLED
//..............................................................
//..............................................................




//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: HVU

void HVU__request_charging(int cap_target_voltage) {
    if( cap_target_voltage >= HVU_MIN_CHARGE_VALUE
        && cap_target_voltage <= HVU_MAX_CHARGE_VALUE ) {

        HVU__requested_ignition_voltage = cap_target_voltage;
        SM_HVU__state = SM_HVU__STATE_CHARGING;
    } else {
        SM_HVU__state = SM_HVU__STATE_CHARGE_FAIL;
        HVU__requested_ignition_voltage = -1;
    }
}

// end: ignition spark & arc unit
//..............................................................
//..............................................................




//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: Serial communication functions

void SERIAL__Parser() {
    static uint8_t cmd[32];         // command buffer
    static uint8_t cmdcount = 0;    // position in the buffer of the received byte
    uint8_t c;                      // received byte
    while(Serial.available()) {
        c = Serial.read();
        if(c > ' ') cmd[cmdcount++] = c;
        if((c == 8) && (cmdcount > 0)) cmdcount--;                // deals with backspaces, if a person on the other side types 
        if((c == 0x0d) || (c == 0x0a) || (cmdcount > 32)) {       // end of command, gets interpreted now
            cmd[cmdcount] = 0;    // clear the last byte in cmd buffer
            if(cmdcount > 2) {    // prevent empty cmd buffer parsing
                switch(cmd[0]) {
                    case 'w': // write
                    SERIAL__ParserWrite(&cmd[1],cmdcount);
                    break;
                    case 'r': // read
                    SERIAL__ParserRead(&cmd[1],cmdcount);
                    break;
                    case 'e': // execute
                    SERIAL__ParserExecute(&cmd[1],cmdcount);
                    break;           
                }    
            }
            cmdcount = 0;
        } 
    }

    while(Serial3.available()) {
        c = Serial3.read();
        if(c > ' ') cmd[cmdcount++] = c;
        if((c == 8) && (cmdcount > 0)) cmdcount--;                // deals with backspaces, if a person on the other side types 
        if((c == 0x0d) || (c == 0x0a) || (cmdcount > 32)) {       // end of command, gets interpreted now
            cmd[cmdcount] = 0;    // clear the last byte in cmd buffer
            if(cmdcount > 2) {    // prevent empty cmd buffer parsing
                switch(cmd[0]) {
                    case 'w': // write
                    SERIAL__ParserWrite(&cmd[1],cmdcount);
                    break;
                    case 'r': // read
                    SERIAL__ParserRead(&cmd[1],cmdcount);
                    break;
                    case 'e': // execute
                    SERIAL__ParserExecute(&cmd[1],cmdcount);
                    break;           
                }    
            }
            cmdcount = 0;
        } 
    }
}


void SERIAL__ParserRead(char * buf,uint8_t cnt) {
    cnt-=4;
    switch (buf[0]) {
        case 'd':
            if(buf[1]=='t') {
                // set date time: 
                // command: wdtYYYY_MM_DD_HH_MM_SS
                // example: wdt2017_9_17_12_30_00
                // note: don't user leading zeros
                //++ Serial.println(DS3231__get_Time());
            }
        break;

        case 'o':
            if(buf[1]=='m') {
                // get current operation mode
                // command: rom
                // returns: returns current operation mode RECOVERY / SCIENCE
                switch(SM_SCU__state_operation_mode) {
                    case SM_SCU__OPERATION_MODE_RECOVERY: 
                        Serial.println("RECOVERY");
                        Serial3.println("RECOVERY");
                    break;
                    case SM_SCU__OPERATION_MODE_NORMAL: 
                        Serial.println("NORMAL");
                        Serial3.println("NORMAL");
                    break;
                }
            }
        break;

        case 'h':
            if(buf[1]=='r') {
                // get system health data from arduino:
                // command: rh
                // parameter: none
                // returns: system health data from arduino
                Serial.println("MCU health log");
                Serial.print("bat v: ");
                //++ Serial.println(powermanager_get_voltage());
                Serial.print("bat a: ");
                //++ Serial.println(powermanager_get_capacity());
                Serial.print("bat charg: ");
                //++ Serial.println(powermanager_get_charge_state());
                Serial.println("frame power: ");
                //++ powermanager_get_usb_power_status() ? Serial.println("FRAME POWER ONLINE") : Serial.println("FRAME POWER OFFLINE");                 

                Serial3.println("MCU health log");
                Serial3.print("bat v: ");
                Serial3.print("bat a: ");
                Serial3.print("bat charg: ");
                Serial3.println("frame power: ");
            }       
        break;

        case 'b':
            if(buf[1]=='v') {
                // get batterie volate:
                // command: rbv
                // returns: voltage in integer value.
                //++ Serial.println(powermanager_get_voltage());
            }

            if(buf[1]=='c') {
                // get batterie capacity:
                // command: rbc
                // returns: milliamper in integer value. 
                //++ Serial.println(powermanager_get_capacity());
            }
        break;


        case 'u':
            if(buf[1]=='s') {
                // get usb power status:
                // command: rus
                // returns: FRAME POWER ONLINE / FRAME POWER OFFLINE
                
                //++ powermanager_get_usb_power_status() ? Serial.println("FRAME POWER ONLINE") : Serial.println("FRAME POWER OFFLINE");             
            }
        break;


        case 't':
            if(buf[1]=='l') {
                // get time left until next datatransfer start:
                // command: rtd
                // returns: returns time left until next datatransfer start in seconds
                // time_t
                //++ Serial.println(TIMEKEEPER__get_deltaT_to_next_datatransfer_seconds());
            }
        break;


        case 'i':
            if(buf[1]=='v') {
                // get current capacitor voltage
                // command: riv
                // returns: returns capacitor voltage
                Serial.println(HVU__get_charge());
                Serial3.println(HVU__get_charge());
            }
        break;


        case 'c':
            if(buf[1]=='r') {
                // TODO: charging error reporting
                switch (SM_HVU__state) {
                    case SM_HVU__STATE_OFF:
                        Serial.println("off");
                        Serial3.println("off");
                        break;
                    case SM_HVU__STATE_CHARGING:
                        Serial.println("charging");
                        Serial3.println("charging");
                        break;
                    case SM_HVU__STATE_CHARGED:
                        Serial.println("charged");
                        Serial3.println("charged");
                        break;
                    case SM_HVU__STATE_CHARGE_FAIL:
                        Serial.println("charg fail");
                        Serial3.println("charg fail");
                        break;
                    case SM_HVU__STATE_IGNITION_FAIL:
                        Serial.println("ignition fail");
                        Serial3.println("ignition fail");
                        break;
                }
            }
        break;

    }
}

void SERIAL__ParserWrite(char * buf,uint8_t cnt) {
    long tmpLong = 0L;
    cnt-=4;
    switch (buf[0]) {
        case 'f': // set front LED PWM
            {
                buf[0] = ' ';
                tmpLong = atoi((const char*)&buf[1]);
                Serial.print("LED front ");
                Serial.println(tmpLong);

                Serial3.print("LED front ");
                Serial3.println(tmpLong);
                POWERLED_frontlight((int)tmpLong);
            }
        break;

        case 'b': // set back LED PWM
            {
                buf[0] = ' ';
                tmpLong = atoi((const char*)&buf[1]);
                Serial.print("LED back ");
                Serial.println(tmpLong);

                Serial3.print("LED back ");
                Serial3.println(tmpLong);
                POWERLED_backlight((int)tmpLong);
            }
        break;

        case 'd':
            if(buf[1]=='t') { // write date time to RTC. example wdt2017_9_16_20_14_00
                String str((const char*)&buf[2]);
                //++ DS3231__set_Time(str,false);
                //++ Serial.println(DS3231__get_Time());
            }
            if(buf[1]=='r') { // write date time to RTC. example wdt2017_9_16_20_14_00
                String str((const char*)&buf[2]);
                //++ DS3231__set_Time(str,true);
                //++ Serial.println(DS3231__get_Time());
            }
        break;

    }
}


void SERIAL__ParserExecute(char * buf,uint8_t cnt) {
    long tmpLong;
    cnt-=4;
    switch (buf[0]) {
        
        case 'r': // serial command "erk300" set the keepalive to 5min
            if(buf[1]=='k') {
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);
                SCU__keepalive_timer = millis()+(1000*tmpLong);
                Serial.println(tmpLong);
                Serial.println("RPikeepalive");

                Serial3.println(tmpLong);
                Serial3.println("RPikeepalive");
            }
            if(buf[1]=='w') {
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);
                SCU__wakeup_timer = millis()+(1000*tmpLong);
                // Serial.println(tmpLong);
                Serial.println("Wakeuptimer set");

                Serial3.println("Wakeuptimer set");
            }
        break;

        case 'i':
            if(buf[1]=='c') { // wct set arc capcitor charg level
                SM_HVU__state = SM_HVU__STATE_CHARGING;
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);
                
                Serial.print("IG CHARG REQUEST ACK ");
                Serial3.print("IG CHARG REQUEST ACK ");
                // Serial.println(tmpLong);
                HVU__request_charging(tmpLong);

                Serial.println(tmpLong);
                Serial3.println(tmpLong);
            }

            if(buf[1]=='a') { 
                Serial.println("CHARG ABORT");

                Serial3.println("CHARG ABORT");
            }

            if(buf[1]=='i') {
                HVU__request_charging(HVU__requested_ignition_voltage);
                SM_HVU__state = SM_HVU__STATE_IGNITE;

                Serial.println(HVU__requested_ignition_voltage);
                Serial3.println(HVU__requested_ignition_voltage);
            }
        break;

    }
}


char SERIAL__readSingleHex(char c) {
    char cret = 0x00;
    if(c>='A' && c<='F') {
        cret = c-'A'+10;
    }
    if(c>='a' && c<='f') {
        cret = c-'a'+10;
    }
    if(c>='0' && c<='9') {
        cret = c-'0';
    }
    return cret;
}


void SERIAL_printLnString(String msg) {
    Serial.println(msg);
    Serial3.println(msg);
}


// end: Serial communication functions
//..............................................................
//..............................................................




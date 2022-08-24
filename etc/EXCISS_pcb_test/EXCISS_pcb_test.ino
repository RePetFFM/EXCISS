#include "config.h"
#include "portmap.h"

// #include <SoftI2CMaster.h>
#include <Wire.h>


// RTC_DS3231 rtc;

uint8_t SCU__OPERATION_MODE = SCU__OPERATION_MODE_NORMAL;



void setup() {
    // wdt_enable(WDTO_8S); // set watchdog interval to 8 seconds

    // analogWriteResolution(16);

    // init serials
    Serial.begin( UART_BAUTRATE );
    Serial.println("serial1 rdy");

    Serial3.begin( UART_BAUTRATE );
    Serial3.println("serial3 rdy");

    
    MCU__init_pins();
    

    // wdt_reset(); // reset watchdog timer
}


void loop() {

    // init usbhub with port 1/2/3 enabled
    digitalWrite( DOUT_USBHUB_PORT_1 , HIGH );
    digitalWrite( DOUT_USBHUB_PORT_2 , LOW );

    
    digitalWrite(DOUT_USBHUB_PWR , USBHUB_PWR_ON);

    digitalWrite( DOUT_USB_X_PWR , USB_ON );
    digitalWrite( DOUT_USB_A_PWR , USB_ON );
    digitalWrite( DOUT_USB_B_PWR , USB_ON );

    // pinMode( DOUT_USBHUB_RESET , USBHUB_RESET );


    // pinMode( DOUT_USBHUB_RESET , USBHUB_NOT_RESET );

    // digitalWrite( DOUT_USB_X_TOGGLE , USB_X_TO_SCU );

    digitalWrite( DOUT_USB_X_TOGGLE , USB_X_TO_ISS );



    Serial.println("DOUT_SCU_SD_TOGGLE XU1100");
    digitalWrite(DOUT_SCU_SD_TOGGLE, SCU_SD_TOGGLE_XU1100);

    delay(2);

    
    digitalWrite(DOUT_SCU_SHUTDOWN_REQUEST, LOW);
    Serial.println("DOUT_SCU_PWR SCU_PWR_ON");
    digitalWrite(DOUT_SCU_PWR, SCU_PWR_ON);
    


    /*
    Serial.println("DOUT_I2CMUX_RST LOW");
    digitalWrite(DOUT_I2CMUX_RST, LOW);
    delay(110);
    Serial.println("DOUT_I2CMUX_RST HIGH");
    digitalWrite(DOUT_I2CMUX_RST, HIGH);
    delay(10);


    Wire.requestFrom(0xE1, 1);
    while(Wire.available())    // slave may send less than requested
    { 
        Serial.println("I2C DAT IN");
        char c = Wire.read();    // receive a byte as character
        Serial.println(c);         // print the character
    }
    */



    // shutdown delay test
    /*
    delay(120000);

    Serial.println("DOUT_SCU_SHUTDOWN_REQUESTed");
    digitalWrite(DOUT_SCU_SHUTDOWN_REQUEST, HIGH);
    */
    // delay(100);
    // digitalWrite(DOUT_SCU_SHUTDOWN_REQUEST, LOW);
    
    Serial.println("GO ");
    while(true) {

    }

    /*

    if(MCU__state == MCU__STATE_INIT) {
        // Serial.println(CORE__init_state,HEX);
        MCU__statemachine_init();
    } 

    if(MCU__state == MCU__STATE_NORMAL) {
    }

    */
}

void MCU__statemachine_init() {
    MCU__init_pins();    
}


void MCU__init_pins() {
    TCCR5B = TCCR5B & 0b11111000 | 0x01; // 0x01 = 31250Hz/1, 0x02 = 31250Hz/8
    
    pinMode( DOUT_SCU_PWR , OUTPUT );
    pinMode( DOUT_SCU_SD_TOGGLE , OUTPUT );

    pinMode( DOUT_SCU_SHUTDOWN_REQUEST , OUTPUT );
    digitalWrite(DOUT_SCU_SHUTDOWN_REQUEST, LOW);

    pinMode( DOUT_USB_X_PWR , OUTPUT );
    pinMode( DOUT_USB_A_PWR , OUTPUT );
    pinMode( DOUT_USB_B_PWR , OUTPUT );

    pinMode( DOUT_USB_X_TOGGLE , OUTPUT);


    pinMode( DOUT_USBHUB_PWR , OUTPUT );
    pinMode( DOUT_USBHUB_PORT_1 , OUTPUT );
    pinMode( DOUT_USBHUB_PORT_2 , OUTPUT );

    pinMode( DOUT_USBHUB_RESET , USBHUB_NOT_RESET );
    digitalWrite( DOUT_USBHUB_RESET , LOW); 


    pinMode( PWMOUT_FRONTLIGHT_CONTROL , OUTPUT);
    analogWrite( PWMOUT_FRONTLIGHT_CONTROL , 0);
    pinMode( PWMOUT_BACKLIGHT_CONTROL , OUTPUT);
    analogWrite( PWMOUT_BACKLIGHT_CONTROL , 0);


    pinMode( DOUT_I2CMUX_RST , OUTPUT );


    // Initialize i2c as master
    Wire.begin(); 
}



/*



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
                switch(SCU__OPERATION_MODE) {
                    case SCU__OPERATION_MODE_RECOVERY: 
                        Serial.println("RECOVERY");
                    break;
                    case SCU__OPERATION_MODE_NORMAL: 
                        Serial.println("NORMAL");
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
                Serial.print("frame power: ");
                //++ powermanager_get_usb_power_status() ? Serial.println("FRAME POWER ONLINE") : Serial.println("FRAME POWER OFFLINE");                 
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
                //++ Serial.println(chargemonitor_get_charge());
            }
        break;


        


        case 'c':
            if(buf[1]=='r') {
                // TODO: charging error reporting
                switch (ignition_status) {
                    case 0:
                        Serial.println("standby");
                        break;
                    case 1:
                        Serial.println("charging");
                        break;
                    case 2:
                        Serial.println("charged");
                        break;
                    case 255:
                        Serial.println("failed");
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
                //++ POWERLED_frontlight((int)tmpLong);
            }
        break;

        case 'b': // set back LED PWM
            {
                buf[0] = ' ';
                tmpLong = atoi((const char*)&buf[1]);
                Serial.print("LED back ");
                Serial.println(tmpLong);
                //++ POWERLED_backlight((int)tmpLong);
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
                science_computer_keepalive_timer = millis()+(1000*tmpLong);
                Serial.println(tmpLong);
                Serial.println("RPikeepalive");
            }
            if(buf[1]=='w') {
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);
                science_computer_wakeup_timer = millis()+(1000*tmpLong);
                Serial.println(tmpLong);
                Serial.println("Wakeuptimer set");
            }
        break;

        case 'i':
            if(buf[1]=='c') { // wct set arc capcitor charg level
                ignition_status = 1;
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);
                tmpLong = 350;
                Serial.print("IG CHARG REQUEST ACK ");
                Serial.println(tmpLong);
                //++ IGNITION_request_charging(tmpLong);
            }

            if(buf[1]=='a') { 
                Serial.println("CHARG ABORT");
            }

            if(buf[1]=='i') {
                //++ IGNITION_request_charging(ignition_requested_charging_voltage);
                ingition_requested = true;
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

void SerialPrint()

*/

// end: Serial communication functions
//..............................................................
//..............................................................




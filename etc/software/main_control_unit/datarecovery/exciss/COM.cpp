/***********************************************************************
* DESCRIPTION :
*       main arduino code for EXCISS version 4, PCB revision 1.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    18 Feb 2018
*
**/

#include "COM.h" 
#include "exciss.h" 





COM::COM() {
}

void COM::begin(int baud) {
    Serial.begin(baud);
    Serial3.begin(baud);
}

void COM::loop() {
    parserSCU();
    parserDEBUG();
}
    
void COM::log(char * aMsg) {
    Serial.print(aMsg);
    Serial3.print(aMsg);
}

void COM::log(char * aMsg, bool aOnlyDebug) {
    Serial.print(aMsg);
    !aOnlyDebug ? Serial3.print(aMsg) : false;
}

void COM::log(int aMsg) {
    Serial.print(aMsg,DEC);
    Serial3.print(aMsg,DEC);
}

void COM::logFloat(float aMsg) 
{
    Serial.print(aMsg);
    Serial3.print(aMsg);
}


void COM::log(String aMsg) {
    Serial.print(aMsg);
    Serial3.print(aMsg);   
}

void COM::log_uint32_t(uint32_t aMsg) {
    Serial.print(aMsg,DEC);
    Serial3.print(aMsg,DEC);   
}

void COM::logBIN(int aMsg) {
    Serial.print(aMsg,BIN);
    Serial3.print(aMsg,BIN);
}

void COM::logBIN(uint16_t aMsg) {
    Serial.print(aMsg,BIN);
    Serial3.print(aMsg,BIN);
}

void COM::logHEX(int aMsg) {
    Serial.print(aMsg,HEX);
    Serial3.print(aMsg,HEX);
}

void COM::newline() {
    Serial.println();
    Serial3.println();
}

//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: Serial communication functions

char COM::ECC(byte repeats, char * buf) {
    char ecc_result = buf[0];
    // Serial.println(buf);
    return ecc_result;
}


void COM::parserDEBUG() {
    static char cmd[32];                                                // command buffer
    static uint8_t cmdcount = 0;                                        // position in the buffer of the received byte
    char ecc_buf[ UART_ECC_LENGTH ];
    static uint8_t ecc_buf_counter = 0;
    char c;                                                             // received byte


    // -------------------
    // parse serial port 1 

    // clear ecc buffer
    ecc_buf_counter = 0;
    for(uint8_t f1; f1<UART_ECC_LENGTH; f1++) {
        ecc_buf[f1] = 0;
    }

    // read serial stream from SCU
    while(Serial.available()) {
        c = Serial.read();
        ecc_buf[ecc_buf_counter++] = c;                                 // push char from serial stream to ecc buffer until ecc repeat count reached
        if(ecc_buf_counter == UART_ECC_LENGTH) {
            c  = ECC(1,ecc_buf);                                        // ecc error correction
            
            if(c > ' ') cmd[cmdcount++] = c;
            if((c == 8) && (cmdcount > 0)) cmdcount--;                  // deals with backspaces, if a person on the other side types 
            if((c == 0x0d) || (c == 0x0a) || (cmdcount > 32)) {         // end of command, gets interpreted now
                cmd[cmdcount] = 0;                                      // clear the last byte in cmd buffer
                if(cmdcount > 2) {                                      // prevent empty cmd buffer parsing

                    com.log("RX_S0 = ");
                    com.log(cmd);
                    com.newline();

                    switch(cmd[0]) {
                        case 'w': // write
                        ParserWrite(&cmd[1],cmdcount);
                        break;
                        case 'r': // read
                        ParserRead(&cmd[1],cmdcount);
                        break;
                        case 'e': // execute
                        ParserExecute(&cmd[1],cmdcount);
                        break;      

                        default:
                        break;     
                    }    

                    cmd[0] = "";

                    cmdcount = 0;
                }
            }
            ecc_buf_counter = 0;
            for(uint8_t f1; f1<UART_ECC_LENGTH; f1++) {
                ecc_buf[f1] = 0;
            }
        }
    }

    if(ecc_buf_counter > 0) { // ecc error: ecc count error
        // do something
    }
    
    

    // -------------------
    // parse serial port 3 

    // clear ecc buffer
    ecc_buf_counter = 0;
    for(uint8_t f1; f1<UART_ECC_LENGTH; f1++) {
        ecc_buf[f1] = 0;
    }
}


void COM::parserSCU() {
    static char cmd[32];                                             // command buffer
    static uint8_t cmdcount = 0;                                        // position in the buffer of the received byte
    char ecc_buf[ UART_ECC_LENGTH ];
    static uint8_t ecc_buf_counter = 0;
    char c;                                                          // received byte


    // -------------------
    // parse serial port 1 

    // clear ecc buffer
    ecc_buf_counter = 0;
    for(uint8_t f1; f1<UART_ECC_LENGTH; f1++) {
        ecc_buf[f1] = 0;
    }

    // read serial stream from SCU
    while(Serial3.available()) {
        c = Serial3.read();
        ecc_buf[ecc_buf_counter++] = c;                                 // push char from serial stream to ecc buffer until ecc repeat count reached
        if(ecc_buf_counter == UART_ECC_LENGTH) {
            c  = ECC(1,ecc_buf);                               // ecc error correction
            
            if(c > ' ') cmd[cmdcount++] = c;
            if((c == 8) && (cmdcount > 0)) cmdcount--;                  // deals with backspaces, if a person on the other side types 
            if((c == 0x0d) || (c == 0x0a) || (cmdcount > 32)) {         // end of command, gets interpreted now
                cmd[cmdcount] = 0;                                      // clear the last byte in cmd buffer
                if(cmdcount > 2) {    // prevent empty cmd buffer parsing

                    com.log("RX_SCU = ");
                    com.log(cmd);
                    com.newline();

                    switch(cmd[0]) {
                        case 'w': // write
                        ParserWrite(&cmd[1],cmdcount);
                        break;
                        case 'r': // read
                        ParserRead(&cmd[1],cmdcount);
                        break;
                        case 'e': // execute
                        ParserExecute(&cmd[1],cmdcount);
                        break;      

                        default:
                        break;     
                    }    

                    cmd[0] = "";

                    cmdcount = 0;
                }
            }
            ecc_buf_counter = 0;
            for(uint8_t f1; f1<UART_ECC_LENGTH; f1++) {
                ecc_buf[f1] = 0;
            }
        }
    }

    if(ecc_buf_counter > 0) { // ecc error: ecc count error
        // do something
    }
    
    

    // -------------------
    // parse serial port 3 

    // clear ecc buffer
    ecc_buf_counter = 0;
    for(uint8_t f1; f1<UART_ECC_LENGTH; f1++) {
        ecc_buf[f1] = 0;
    }
}


void COM::ParserRead(char * buf,uint8_t cnt) {
    long tmpLong = 0L;

    cnt-=4;
    switch (buf[0]) {
        case 'd':
            if(buf[1]=='t') {
                // command: rdt
                // read date time: 
                // return:
                // RX_S0 = rdt
                // [ TIMEKEEPER ]
                // current_unix_timestamp = 1500770250
                // current_datetime = 2017-7-23T0:37:30

                TIMEKEEPER__log_current_datetime();
            }

            if(buf[1]=='m') {
                com.log("[ MCU ]");
                com.newline();
                if( TIMEKEEPER__datatransfer_window_enable_no() > TIMEKEEPER__datatransfer_window_enable_yes() )
                {
                    com.log("debug mode is enabled");
                    com.newline();
                    com.log("turned off datatransfer window");
                } else {
                    com.log("debug mode is disabled");
                    com.newline();
                    com.log("datatransfer window activated");
                }
                
                com.newline();

                TIMEKEEPER__logout_datatransfer_window_enable_flag();
            }
        break;

        case 'o':
            if(buf[1]=='m') {
                // get current operation mode
                // command: rom
                // returns: returns current operation mode RECOVERY / SCIENCE

                SCU__logout_current_mode();
                
                uint32_t next_datatransfer_seconds = TIMEKEEPER__delta_to_next_datatransfer_seconds();

                com.log("next_datatransfer_seconds = ");
                com.log_uint32_t(next_datatransfer_seconds);
                com.newline();
                com.log("compiled_datetime = ");
                com.log(COMP_DATE);
                com.log(" ");
                com.log(COMP_TIME);
                com.newline();
            }
        break;

        case 's':
            if(buf[1]=='c') {
                POWER__logout_charger();
            }

            if(buf[1]=='g') {
                POWER__logout_gauge();
            }

            if(buf[1]=='t') {
                // read all temperature sensors

                if(buf[2]=='a') {
                    TEMPSENS__logout_all_temperatur_sensors();
                } else {
                    buf[0] = ' ';
                    buf[1] = ' ';
                    tmpLong = atoi((const char*)&buf[2]);
                    
                    TEMPSENS__logout_temperatur_sensor(tmpLong);
                }
                
            }

            if(buf[1]=='p') {
                // read all power switches
                POWER__logout_power_switches();
            }

            if(buf[1]=='i')
            {
                busMaster.logoutI2CLockState();
            }
        break;

        case 'i':
            if(buf[1]=='v') {
                // get current capacitor voltage
                // command: riv
                // returns: returns capacitor voltage

                int tmpHVUM = HVU__get_charge();
                com.log("[ HVU ]");
                com.newline();
                com.log("charg_voltage = ");
                com.log(tmpHVUM);
                com.newline();
            }
            if(buf[1]=='s') {
                // get current ignition status
                // command: ris
                // returns: returns capacitor voltage

                HVU__logout_status();
            }
        break;

        case 't':
            if(buf[1]=='a')
            {
                MCU__log_timer_status();
            }
        break;

        default:
        break;

    }
}

void COM::ParserWrite(char * buf,uint8_t cnt) {
    long tmpLong = 0L;
    cnt-=4;
    switch (buf[0]) {
        case 'l': // set front LED PWM
            if(buf[1]=='f') {
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);

                LIGHTS__front_led_pwm( (int)tmpLong );
            }

            if(buf[1]=='b') {
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);

                LIGHTS__back_led_pwm( (int)tmpLong );
            }
        break;

        case 'd':
            if(buf[1]=='n') { // write date time to RTC. example wdn2017_9_16_20_14_00
                String str((const char*)&buf[2]);
                TIMEKEEPER__set_time(str,false);

                TIMEKEEPER__log_current_datetime();
            }
            if(buf[1]=='r') { // write date time to RTC plus power on time. example wdr2017_9_16_20_14_00
                String str((const char*)&buf[2]);
                TIMEKEEPER__set_time(str,true);
                
                TIMEKEEPER__log_current_datetime();
            }
        break;

        case 'u':
            if(buf[1]=='x') {

                buf[2]=='1' ? USBSTORAGE__x_on() : USBSTORAGE__x_off();

            }

            if(buf[1]=='a') {

                buf[2]=='1' ? USBSTORAGE__a_on() : USBSTORAGE__a_off();                            
            }

            if(buf[1]=='b') {

                buf[2]=='1' ? USBSTORAGE__b_on() : USBSTORAGE__b_off();
                
            }
        break;

        case 'p':
            if(buf[1]=='s') {
                buf[2]=='1' ? POWER__mode_start_science_experiment() : POWER__mode_end_science_experiment();
            }
        break;

        case 's':
            if(buf[1]=='d') {
                if(buf[2]=='1') {
                    SCU__sd_card_to_XU1100();
                }

                if(buf[2]=='2') {
                    SCU__sd_card_to_XU1200();
                }
            }
        break;


        case'v':
            if(buf[1]=='s') {
                // Sets the vibration motor driver effect sequence.
                // Some example sequence:
                // wvs58_10_10_10_58_10_10_10   <--- the default sequence
                // wvs58_58_58_58_58_58_58_58
                // wvs34_00_00_00_00_00_00_00
                char * tmpChr = &buf[2];
                VIBRATION_MOTOR__setup(tmpChr);
            }
        break;

        case'i':
            if(buf[1]=='t') {
                // write ignition threshold
                // wit[0-30] value is Volts
                
                tmpLong = atoi((const char*)&buf[2]);

                HVU__set_ignition_delta( (int)tmpLong );
            }
        break;


        default:
        break;

    }
}


void COM::ParserExecute(char * buf,uint8_t cnt) {
    long tmpLong;
    cnt-=4;
    switch (buf[0]) {

        case 'v':
            if(buf[1]=='g') {
                VIBRATION_MOTOR__go();
            }

            if(buf[1]=='s') {
                VIBRATION_MOTOR__stop();
            }

            if(buf[1]=='t') {
                VIBRATION_MOTOR__test();
            }
        break;
        
        case 't': 
            if(buf[1]=='k') {
                // serial command "etk300" set the keepalive to 5min
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);

                if( tmpLong > 0) {
                    com.log("[ MCU ]");
                    com.newline();
                    com.log("set_RPikeepalive = ");
                    com.log(tmpLong);
                    com.newline();

                    MCU__set_science_scu_keep_alive( tmpLong );    
                }                
            }

            if(buf[1]=='w') {
                // serial command "etw300" set the keepalive to 5min
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);
                
                if( tmpLong > 0) {
                    com.log("[ MCU ]");
                    com.newline();
                    com.log("set_Wakeuptimer = ");
                    com.log(tmpLong);
                    com.newline();

                    MCU__set_science_scu_wakup( tmpLong );
                }
            }
        break;

        case 'i':
            if(buf[1]=='v') { 
                // charg capacitors with requested target voltage
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);

                HVU__start_charging(tmpLong);
            }

            if(buf[1]=='a') { 
                // abort charging
                // dump cap energy
                HVU__abort();
            }

            if(buf[1]=='i') {
                // requesting ignition
                buf[0] = ' ';
                buf[1] = ' ';
                tmpLong = atoi((const char*)&buf[2]);

                HVU__ignite_at_charg(tmpLong);

            }

            if(buf[1]=='t') {
                // test ignition without arc
                HVU__do_test_ignition_without_arc();
            }
        break;

        case 'r':
            if(buf[1]=='4' && buf[2]=='2') {
                com.log("[ MCU ]");
                com.newline();
                com.log("force_watchdog_reset");
                com.newline();
                while(1) {
                    com.log(".");
                    delay(1000);
                }
                com.log("no watchdog reset executed");
                com.newline();
            }
        break;

        case 'd':
            if(buf[1]=='m' && buf[2]=='1') {
                buf[0] = ' ';
                buf[1] = ' ';
                com.log("[ MCU ]");
                com.newline();
                com.log("debug mode enabled");
                com.newline();
                com.log("turn off datatransfer window");
                com.newline();

                TIMEKEEPER__disable_datatransfer_window();
            }

            if(buf[1]=='m' && buf[2]=='0') {
                buf[0] = ' ';
                buf[1] = ' ';
                com.log("[ MCU ]");
                com.newline();
                com.log("debug mode disabled");
                com.newline();
                com.log("datatransfer window active");
                com.newline();

                TIMEKEEPER__enable_datatransfer_window();
            }
        break;

        default:
        break;

    }
}


char * COM::intToHex(int a_i) {
    // snprintf
}


char COM::readSingleHex(char c) {
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




// end: Serial communication functions
//..............................................................
//..............................................................

/***********************************************************************
* FILENAME :        HVU_controller.ino             
*
* DESCRIPTION :
*       main arduino code for EXCISS ignition testing device.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    18 Feb 2018
*
**/



#include "config.h"
#include "portmap.h"
#include "hvu.h"



int hvu_target_voltage = 0;

int hvu_trigger_repeat  = 0;

int hvu_trigger_count   = 0;

int hvu_trigger_delat = 0;

void setup() {

    // init serials
    Serial.begin( UART_BAUTRATE );
    Serial.println("serial rdy");
 
    MCU__init_pins();  
    while(!digitalRead(DIN_BTN_START_PAUSE)){
        // statement
    }
}

bool triggered = false;


void loop() {
    

    // SERIAL__Parser();


    Serial.println(HVU__get_charge());

    if( !digitalRead(DIN_BTN_START_PAUSE) ) {
        digitalWrite(DOUT_HVU_TRIGGER , HIGH );

        delay(100);

        digitalWrite( DOUT_HVU_TRIGGER , LOW );

        triggered = true;
    }


    if(triggered) {
        digitalWrite(13, HIGH);
        delay(100);
        digitalWrite(13, LOW);            
        delay(100);
    }

}

void MCU__init_pins() {
    pinMode( DIN_BTN_START_PAUSE , INPUT_PULLUP );

    pinMode( DIN_ENC_SWITCH , INPUT);
    pinMode( DIN_ENC_CHANNELA , INPUT);
    pinMode( DIN_ENC_CHANNELB , INPUT);
    pinMode( DOUT_ENC_LEDRED , OUTPUT );
    pinMode( DOUT_ENC_LEDGREEN , OUTPUT );
    pinMode( DOUT_ENC_LEDBLUE , OUTPUT );
    digitalWrite( DOUT_ENC_LEDRED , HIGH );
    digitalWrite( DOUT_ENC_LEDGREEN , HIGH );
    digitalWrite( DOUT_ENC_LEDBLUE , LOW );


    
    pinMode( DOUT_HVU_ENABLE , OUTPUT );
    digitalWrite( DOUT_HVU_600V_ENABLE , HIGH );

    pinMode( DOUT_HVU_600V_ENABLE , OUTPUT );
    digitalWrite( DOUT_HVU_600V_ENABLE , HIGH );

    pinMode( DOUT_HVU_TRIGGER , OUTPUT );
    digitalWrite( DOUT_HVU_TRIGGER , LOW );

    pinMode( AIN_HVU_MEASURE , INPUT );
}




//--------------------------------------------------------------
//--------------------------------------------------------------
// begin: HVU

void HVU__request_charging(int cap_target_voltage) {
    if( cap_target_voltage >= HVU_MIN_CHARGE_VALUE
        && cap_target_voltage <= HVU_MAX_CHARGE_VALUE ) {

        
        HVU__start_charging(cap_target_voltage);
        
    } else {
        
        HVU__start_charging(0);
    }
}

// end: ignition spark & arc unit
//..............................................................
//..............................................................





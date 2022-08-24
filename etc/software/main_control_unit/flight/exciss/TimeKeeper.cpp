/***********************************************************************
* DESCRIPTION :
*
* 
* AUTHOR :    Shintaro Fujita
*
**/

#include "config_TimeKeeper.h"
#include "TimeKeeper.h"
#include "exciss.h"
#include "RTClib.h"



RTC_DS3231 rtc;

// byte TIMEKEEPER__status = 0x00;

unsigned long _timekeeper__bootup_unixtimestamp;

uint8_t _timekeeper_datatransfer_window_enabled_a = 0xAA;
uint8_t _timekeeper_datatransfer_window_enabled_b = 0xAA;
uint8_t _timekeeper_datatransfer_window_enabled_c = 0xAA;
uint8_t _timekeeper_datatransfer_window_enabled_d = 0xAA;
uint8_t _timekeeper_datatransfer_window_enabled_e = 0xAA;

struct TIMEKEEPER__ALERT _timekeeper__alerts[ TIMEKEEPER__TIMEREVENT_ARRAY_COUNT ];

void TIMEKEEPER__init() {

    // busMaster.switchMuxPortByDeviceAddress(0x68);

    delay(10);


    if (! rtc.begin()) {
        com.log("ERROR Couldn't find RTC");
        // TIMEKEEPER__status = 0xEE;
    } else {
        com.log("rtc ready");
        // TIMEKEEPER__status = 0x01;
    }

    unsigned long _ts_mcu_millis = millis();
    DateTime currentDateTime = rtc.now();

    _timekeeper__bootup_unixtimestamp = currentDateTime.unixtime();

    _timekeeper__bootup_unixtimestamp = _timekeeper__bootup_unixtimestamp - ( _ts_mcu_millis * 1000UL );


    // -------------------------------------
    // tempsensor read out
    // used by:
    // - mcu
    _timekeeper__alerts[ TIMEKEEPER__TIMEREVENT_ARRAY_INDEX_TEMPSENS_UPDATE ].enabled = false;
    _timekeeper__alerts[ TIMEKEEPER__TIMEREVENT_ARRAY_INDEX_TEMPSENS_UPDATE ].interval = 1000UL;
    _timekeeper__alerts[ TIMEKEEPER__TIMEREVENT_ARRAY_INDEX_TEMPSENS_UPDATE ].timestamp = 5000UL;
    _timekeeper__alerts[ TIMEKEEPER__TIMEREVENT_ARRAY_INDEX_TEMPSENS_UPDATE ].eventID = TIMEKEEPER__TIMEREVENT_TEMPSENS_UPDATE;
    
}


void TIMEKEEPER__healthcheck() {
    // busMaster.switchMuxPortByDeviceAddress(0x68);
    com.log("[ TIMEKEEPER ]");
    com.newline();
    DateTime currentDateTime = rtc.now();

    uint32_t currentUnixTime = currentDateTime.unixtime();
    com.log("current_unix_timestamp = ");
    com.log_uint32_t(currentUnixTime);
    com.newline();

    com.log("current_datetime = ");
    com.log(TIMEKEEPER__get_DateTime_String());
    com.newline();
}


uint8_t TIMEKEEPER__check_alert_event() {
    uint8_t alert_eventID = 0x0;
    alert_eventID = 0x0;
    for(uint8_t i; i < TIMEKEEPER__TIMEREVENT_ARRAY_COUNT ; i++) {
        if(_timekeeper__alerts[i].enabled 
            && _timekeeper__alerts[i].timestamp<=millis()) {
            
            alert_eventID = _timekeeper__alerts[i].eventID;

            // is this a interval timer?
            if(_timekeeper__alerts[i].interval > 0) {
                _timekeeper__alerts[i].timestamp = millis() + _timekeeper__alerts[i].interval; // set next event time
            } else {
                _timekeeper__alerts[i].enabled = false; // it's not a interval timer. deactiate timer.
            }

            return alert_eventID;
        }
    }
    return alert_eventID;
}


void TIMEKEEPER__log_current_datetime() {
    
    com.log("[ TIMEKEEPER ]");
    com.newline();
    DateTime currentDateTime = rtc.now();

    uint32_t currentUnixTime = currentDateTime.unixtime();
    com.log("current_unix_timestamp = ");
    com.log_uint32_t(currentUnixTime);
    com.newline();

    com.log("current_datetime = ");
    com.log(TIMEKEEPER__get_DateTime_String());
    com.newline();   
}

void TIMEKEEPER__log_all()
{
    
}

String TIMEKEEPER__get_DateTime_String() {
    
    DateTime now = rtc.now();
    String current_Time = String( now.year());

    // YYYY-MM-DDThh:mm:ssTZD

    current_Time += "-" + String( now.month());
    current_Time += "-" + String( now.day());
    current_Time += "T" + String( now.hour());
    current_Time += ":" + String( now.minute());
    current_Time += ":" + String( now.second());
    return current_Time;
}


void TIMEKEEPER__set_time(String new_Time, bool timeRecoverMode) {
    
    int rtc_Time[] = {0,0,0,0,0,0};

    int j = 0;
    int pos = 0;
    for (int i = 0; i < new_Time.length(); i++) {
        if ((char)new_Time[i] == '_'&& j<6) {
            rtc_Time[j] = new_Time.substring(pos, i).toInt();
            j++;
            pos=i+1;
        }
        if (i==new_Time.length()-1&& j<6) {
            rtc_Time[j] =  new_Time.substring(pos, new_Time.length()).toInt();
        }
    }

    if(timeRecoverMode) {
        if (j<6) {
            // recover_start_datetime_ut + recover_mode_last_entered_ut;

            uint32_t last_poweron_timestamp_in_seconds = millis() / 1000UL;

            DateTime recover_date_time_unixtime = DateTime((uint16_t)rtc_Time[0],rtc_Time[1],rtc_Time[2],rtc_Time[3],rtc_Time[4],rtc_Time[5]);

            uint32_t recover_current_date_time = recover_date_time_unixtime.unixtime()+last_poweron_timestamp_in_seconds;

            rtc.adjust(DateTime(recover_current_date_time));
        }
    } else {
        if (j<6) rtc.adjust(DateTime((uint16_t)rtc_Time[0],rtc_Time[1],rtc_Time[2],rtc_Time[3],rtc_Time[4],rtc_Time[5]));
    }

}


uint32_t TIMEKEEPER__timer_remaining_time(uint8_t aEventID)
{
    uint32_t remaining_time = 0x0;

    for(uint8_t i; i < TIMEKEEPER__TIMEREVENT_ARRAY_COUNT ; i++) {
        if( _timekeeper__alerts[ i ].enabled && _timekeeper__alerts[ i ].eventID==aEventID ) {
            millis()>_timekeeper__alerts[ i ].timestamp ? remaining_time = 0x0 : remaining_time =  _timekeeper__alerts[ i ].timestamp - millis();
            break;
        }
    }

    return remaining_time;
}


uint32_t TIMEKEEPER__delta_to_next_datatransfer_seconds() 
{
    // busMaster.switchMuxPortByDeviceAddress(0x68);
    DateTime cdt = rtc.now();
    uint32_t current_date_time_ut = cdt.unixtime();
    long hour = cdt.hour();
    long minute = cdt.minute();

    uint32_t result = 0UL;

    if(hour > TIMEKEEPER__DATATRANSFER_WINDOW_END_HOUR) {
        result = DateTime(cdt.year(),cdt.month(),cdt.day()+1,TIMEKEEPER__DATATRANSFER_WINDOW_START_HOUR,0,0).unixtime()-current_date_time_ut;
    }

    if(hour < TIMEKEEPER__DATATRANSFER_WINDOW_START_HOUR) {
        result = DateTime(cdt.year(),cdt.month(),cdt.day(),TIMEKEEPER__DATATRANSFER_WINDOW_START_HOUR,0,0).unixtime()-current_date_time_ut;
    }

    return result;
}

uint8_t TIMEKEEPER__ok_to_bootup_scu() 
{
    uint8_t result = 0xAA;

    result = 0xAA;

    uint32_t delta_to_next_datatransfer_seconds = TIMEKEEPER__delta_to_next_datatransfer_seconds();

    if( delta_to_next_datatransfer_seconds <= TIMEKEEPER__DATATRANSFER_WINDOW_START_MARGIN_SECONDS ) 
    {
        result = 0x55;
    }

    if( delta_to_next_datatransfer_seconds <= 1800UL ) 
    {
        result = 0x55;
    }

    if( TIMEKEEPER__datatransfer_window_enable_yes() < TIMEKEEPER__datatransfer_window_enable_no() )
    {
        result = 0xAA;
    }

    return result;
}

unsigned long TIMEKEEPER__get_current_unix_timestamp()
{
    unsigned long result_uts;

    return result_uts;
}

void TIMEKEEPER__enable_datatransfer_window() {
    _timekeeper_datatransfer_window_enabled_a = 0xAA;
    _timekeeper_datatransfer_window_enabled_b = 0xAA;
    _timekeeper_datatransfer_window_enabled_c = 0xAA;
    _timekeeper_datatransfer_window_enabled_d = 0xAA;
    _timekeeper_datatransfer_window_enabled_e = 0xAA;
}

void TIMEKEEPER__disable_datatransfer_window() {
    _timekeeper_datatransfer_window_enabled_a = 0x55;
    _timekeeper_datatransfer_window_enabled_b = 0x55;
    _timekeeper_datatransfer_window_enabled_c = 0x55;
    _timekeeper_datatransfer_window_enabled_d = 0x55;
    _timekeeper_datatransfer_window_enabled_e = 0x55;
}

uint8_t TIMEKEEPER__datatransfer_window_enable_yes() 
{
    uint8_t yesVotes = 0;
    _timekeeper_datatransfer_window_enabled_a == 0xAA ? yesVotes++ : false;
    _timekeeper_datatransfer_window_enabled_b == 0xAA ? yesVotes++ : false;
    _timekeeper_datatransfer_window_enabled_c == 0xAA ? yesVotes++ : false;
    _timekeeper_datatransfer_window_enabled_d == 0xAA ? yesVotes++ : false;
    _timekeeper_datatransfer_window_enabled_e == 0xAA ? yesVotes++ : false;
    return yesVotes;
}

uint8_t TIMEKEEPER__datatransfer_window_enable_no() 
{
    uint8_t noVotes = 0;
    _timekeeper_datatransfer_window_enabled_a == 0x55 ? noVotes++ : false;
    _timekeeper_datatransfer_window_enabled_b == 0x55 ? noVotes++ : false;
    _timekeeper_datatransfer_window_enabled_c == 0x55 ? noVotes++ : false;
    _timekeeper_datatransfer_window_enabled_d == 0x55 ? noVotes++ : false;
    _timekeeper_datatransfer_window_enabled_e == 0x55 ? noVotes++ : false;
    return noVotes;
}

void TIMEKEEPER__logout_datatransfer_window_enable_flag()
{
    com.log("[ TIMEKEEPER ]");
    com.newline();
    com.log("a = ");
    com.logHEX(_timekeeper_datatransfer_window_enabled_a);

    com.newline();
    com.log("b = ");
    com.logHEX(_timekeeper_datatransfer_window_enabled_b);

    com.newline();
    com.log("c = ");
    com.logHEX(_timekeeper_datatransfer_window_enabled_c);

    com.newline();
    com.log("d = ");
    com.logHEX(_timekeeper_datatransfer_window_enabled_d);

    com.newline();
    com.log("e = ");
    com.logHEX(_timekeeper_datatransfer_window_enabled_e);

    com.newline();
}
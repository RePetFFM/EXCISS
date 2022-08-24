/***********************************************************************
*
* DESCRIPTION :
* AUTHOR :    Shintaro Fujita
*
**/

#include "TEMPSENS.h"
#include "exciss.h"
#include "OneWire.h"
#include "DallasTemperature.h"


static float _tempsens__temperatures[15];

static DeviceAddress _tempsens_address[15];

uint8_t _tempsens__number_sensors;

OneWire _tempsens__oneWire;
// OneWire oneWire(2) // Use address 20
DallasTemperature _tempsens__sensors(&_tempsens__oneWire);

void TEMPSENS__init()
{

    bool b = busMaster.switchMuxPortByDeviceAddress(0x18); // this is where the Bridge is

    delay(10);

    _tempsens__sensors.begin();

    // async mode
    _tempsens__sensors.setWaitForConversion(true);

    delay(100);

    DeviceAddress currAddress;
    _tempsens__number_sensors = _tempsens__sensors.getDeviceCount();

    for (uint8_t i=0; i<_tempsens__number_sensors; i++)
    {
        _tempsens__temperatures[i] = 0.0;
        _tempsens__sensors.getAddress(_tempsens_address[i], i);
    }

    // _tempsens__sensors.requestTemperatures();

}

uint8_t TEMPSENS__sensor_conversion_complete() 
{
    // non blocking code to check converseion status

    static uint8_t sm = 0; // initial state machine value
    static unsigned long timeout_start = 0UL;
    static unsigned long timeout_delay = 50UL; // default timeout 50 millis
    uint8_t status = 0; 

    switch(sm) {
        case 0:
            status = 0;
            timeout_start = millis(); // reset timeout

        case 1: // wait on busy
            if(_tempsens__oneWire.readStatus() & DS2482_STATUS_BUSY)
            {
                // bus is busy
                break;  
            } 
            else 
            {
                sm = 2;
            }

            if( millis() - timeout_start > timeout_delay ) 
            {
                // timeout occured reset statemachine
                sm = 0;
                break;
            }
        
        case 2:
            timeout_start = millis();
            _tempsens__oneWire.wireWriteBitInstant(1);
            sm = 3;

        case 3:
            status = _tempsens__oneWire.readStatus();
            if( !(status & DS2482_STATUS_BUSY) )
            {
                status = status & DS2482_STATUS_SBR ? 1 : 0;
                sm = 0;
            } else {
                status = 0;
            }
            if( millis() - timeout_start > timeout_delay )
            {
                // timeout occured reset statemachine
                sm = 0;
                status = 0;
                break;
            }

    }
    
    return status;
}

void TEMPSENS__update_queed_temeperatur_sensor()
{
    bool b = busMaster.switchMuxPortByDeviceAddress(0x18); // this is where the Bridge is


    static uint8_t current_sensor_index = 0;

    // _tempsens__sensors.requestTemperatures();


    if( TEMPSENS__sensor_conversion_complete() ) { // isConversionComplete
        // _tempsens__temperatures[ current_sensor_index ] = _tempsens__sensors.getTempC(_tempsens_address[current_sensor_index]);

        current_sensor_index < _tempsens__number_sensors ? current_sensor_index++ : current_sensor_index = 0;

        // _tempsens__sensors.requestTemperaturesByAddress(_tempsens_address[current_sensor_index]);    

    }

    

}

void TEMPSENS__logout_all_temperatur_sensors()
{
    bool b = busMaster.switchMuxPortByDeviceAddress(0x18); // this is where the Bridge is


    _tempsens__sensors.requestTemperatures(); // <-- blocking
    

    com.log("[ TEMPSENS ]");
    com.newline();
    for (uint8_t i=0; i<_tempsens__number_sensors; i++)
    {

        wdt_reset();

        _tempsens__temperatures[ i ] = _tempsens__sensors.getTempC(_tempsens_address[ i ]);

          
        com.log("sensor_addr = ");
        for (uint8_t j = 0; j < 15; j++)
        {
            // zero pad
            if (_tempsens_address[i][j] < 16) com.log("0");

            com.logHEX(_tempsens_address[i][j]);
            if (j<15) com.log(",");
        }

        com.newline();

        com.log("sensor_temperature = ");
        com.logFloat(_tempsens__temperatures[ i ]);
        com.newline();
    }
}


void TEMPSENS__logout_temperatur_sensor(uint8_t aId)
{
    bool b = busMaster.switchMuxPortByDeviceAddress(0x18); // this is where the Bridge is


    _tempsens__sensors.requestTemperatures();
    
    com.log("[ TEMPSENS ]");
    com.newline();

    if( aId>=0 && aId<15) 
    {
        _tempsens__temperatures[ aId ] = _tempsens__sensors.getTempC(_tempsens_address[ aId ]);

          
        com.log("sensor_addr = ");
        for (uint8_t j = 0; j < 15; j++)
        {
            // zero pad
            if (_tempsens_address[ aId ][j] < 16) com.log("0");

            com.logHEX(_tempsens_address[ aId ][j]);
            if (j<15) com.log(",");
        }

        com.newline();

        com.log("sensor_temperature = ");
        com.logFloat(_tempsens__temperatures[ aId ]);
        com.newline();
    } 
    else
    {
        com.log("error: id out of range");
        com.newline();
    }
}

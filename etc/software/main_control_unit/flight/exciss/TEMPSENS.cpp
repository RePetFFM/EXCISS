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
DallasTemperature _tempsens__sensors(&_tempsens__oneWire);


void TEMPSENS__init()
{
    // reads sensor address from eeprom.
    // address data begins with at eeprom address 10 with the numbers of sensors.
    // every sensor takes 8 bytes from the eeprom.

    com.log("[ TEMPSENS ]");
    com.newline();
    com.log("start init");
    com.newline();

    _tempsens__number_sensors = NONVOLATILEMEM__read(10);

    _tempsens__number_sensors > 20 ? _tempsens__number_sensors = 20 : false; // limits the maximum sensor count

    uint16_t eepromStart = 0;

    for (int i=0; i < _tempsens__number_sensors; i++)
    {
        eepromStart = 11+(8*i);

        _tempsens__temperatures[i] = 0.0;

        TEMPSENS__read_address_from_eeprom(eepromStart, _tempsens_address[i]);

        TEMPSENS__printAddress(i);
        com.newline();
    }
    
    com.log("init done");
    com.newline();

}


void TEMPSENS__read_address_from_eeprom(uint16_t aEepromStart, DeviceAddress aAddress)
{
  for (uint16_t i = 0; i < 8; i++)
  {
    aAddress[i] = NONVOLATILEMEM__read(aEepromStart + i);
  } 
}


void TEMPSENS__printAddress(uint8_t index)
{
  com.log("{ ");
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    com.log("0x");
    if (_tempsens_address[index][i] < 16) com.log("0");
    com.logHEX(_tempsens_address[index][i]);
    if (i<7) com.log(", ");
    
  }
  com.log(" }");
}


uint8_t TEMPSENS__sensor_conversion_complete() 
{
    // non blocking code to check converseion status
    // currently not used

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


void TEMPSENS__logout_all_temperatur_sensors()
{    
    _tempsens__sensors.requestTemperatures(); // <-- blocking

    delay(500); // required time to aquire temperature messurments
    
    com.log("[ TEMPSENS ]");
    com.newline();

    for (uint8_t i=0; i<_tempsens__number_sensors; i++)
    {
        wdt_reset();

        _tempsens__temperatures[ i ] = _tempsens__sensors.getTempC(_tempsens_address[ i ]);
          
        com.log("sensor_addr = ");
        TEMPSENS__printAddress(i);
        com.newline();

        com.log("sensor_temperature = ");
        com.logFloat(_tempsens__temperatures[ i ]);
        com.newline();
    }
}


void TEMPSENS__logout_temperatur_sensor(uint8_t aId)
{
    
    _tempsens__sensors.requestTemperatures();

    delay(500); // required time to aquire temperature messurments
    
    com.log("[ TEMPSENS ]");
    com.newline();

    wdt_reset();

    if( aId>=0 && aId<15) 
    {
        _tempsens__temperatures[ aId ] = _tempsens__sensors.getTempC(_tempsens_address[ aId ]);

        com.log("sensor_addr = ");
        TEMPSENS__printAddress(aId);
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

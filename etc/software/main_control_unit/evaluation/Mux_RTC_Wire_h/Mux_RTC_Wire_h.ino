/***********************************************************************
* FILENAME :        Mux_RTC.cpp      DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* This version uses the Arduino Wire.h to compare it to the version without Wire.h.
*
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Rainer Haseitl
*
**/

#include "BusMaster.h"
#include "RTClib.h"

BusMaster busMaster;
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() 
{
  // initilize BusMaster
  busMaster = BusMaster();
  busMaster.initBus();

  bool b = busMaster.switchMuxPortByDeviceAddress(0x68); // RTC


  Serial.begin(9600);

  delay(3000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // set the time:
  //Serial.println("setup() - adjustTime");
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  
  if (rtc.lostPower()) { // this function does work, when the coin cell is removed and the board is power cycled, this is detected!
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  else
  {
    Serial.println("RTC did not loose power, everything ok!");
  }
  
}


void loop() 
{
    DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    /*Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    
    // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));
    
    Serial.print(" now + 7d + 30s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
    
    Serial.println(); */
    delay(3000);
}


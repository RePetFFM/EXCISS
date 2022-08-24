#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("ERROR Couldn't find RTC");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  set_Time("2000_7_16_12_23_56");
  delay (3000);
  Serial.println(get_Time());

  
}

/*Returns the RTC Time as String YYYY_M_D_H_M_S*/
String get_Time() {
  DateTime now = rtc.now();
  String current_Time = String( now.year());
  
  current_Time += "_" + String( now.month());
  current_Time += "_" + String( now.day());
  current_Time += "_" + String( now.hour());
  current_Time += "_" + String( now.minute());
  current_Time += "_" + String( now.second());
  return current_Time;
}

/*Sets the rtc to a new time. Sring format is: 
 * YYYY_M_D_H_M_S (no leading 0s)
 */
void set_Time(String new_Time) {

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

  if (j<6) rtc.adjust(DateTime((uint16_t)rtc_Time[0],rtc_Time[1],rtc_Time[2],rtc_Time[3],rtc_Time[4],rtc_Time[5]));
}


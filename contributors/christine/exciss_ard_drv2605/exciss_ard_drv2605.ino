#include <Wire.h>
#include "Adafruit_DRV2605.h"
#include "elapsedMillis.h"

//Globals for DRV2605_Haptic
Adafruit_DRV2605 drv;
elapsedMillis time_Elapsed;
unsigned long start_ms;
int duration_ms;


void setup() {
  Serial.begin(9600);
    
  // Configuration for DRV2605_Haptic I2C trigger Modus
  drv.begin();
  drv.setMode(DRV2605_MODE_INTTRIG); // default, internal trigger when sending GO command
  drv.selectLibrary(1);
  drv.useERM();
  //drv.useLRA(); THE ADAFRUIT THING LOOKS LIKE LRA but really is an ERM. I checked.

  //Test
  /*
  delay(500);
  Serial.print("Config ok?: ");
  Serial.println( set_Config_Drv("64_64_64_64_64_64_64"));
  delay(500);
  Serial.print("Run it for 5 secs ok?: ");
  Serial.println(set_Run_Drv("5000"));
  */
}

void loop() {
    
  run_Drv();
   
}

/*
 * Sets the config new.
 */
void config_Drv(uint8_t form_1, uint8_t form_2, uint8_t form_3, uint8_t form_4, uint8_t form_5, uint8_t form_6, uint8_t form_7 ) {
  drv.setWaveform(0, form_1);  
  drv.setWaveform(1, form_2);  
  drv.setWaveform(2, form_3);  
  drv.setWaveform(3, form_4);  
  drv.setWaveform(4, form_5);  
  drv.setWaveform(5, form_6);  
  drv.setWaveform(6, form_7);  // end 
}

/*
 * Runs the Drv.
 */
bool run_Drv() {
  if (time_Elapsed-start_ms < duration_ms) { drv.go(); return 1;}
  else { drv.stop(); return 0;}
}


/*
 * Set a waveform configuration of exactly 7 values of 0-116.
 * Example "64_64_64_64_14_14_0". 0 means stop.
 * The waveform is looped by run_Drv().
 */
bool set_Config_Drv (String my_Config) {

  int drv_Config[] = {0,0,0,0,0,0,0};

  //Split String and get Int values
  int j = 0;
  int pos = 0;
  for (int i = 0; i < my_Config.length(); i++) {
    if ((char)my_Config[i] == '_'&& j<7) {
      drv_Config[j] = my_Config.substring(pos, i).toInt();
      j++;
      pos=i+1;
    }
    //Last substring
    if (i==my_Config.length()-1 && j<7) {
      drv_Config[j] = my_Config.substring(pos, my_Config.length()).toInt();
    }
  }

  //Check if the values are ok
  bool my_Config_OK = true;
  if (j!=6) my_Config_OK = false;
  for (int i=0; i<7;i++){
    if (drv_Config [i] <0  || drv_Config [i] > 116) my_Config_OK = false;
  }

  //Set Config
  if (my_Config_OK) {
    config_Drv(drv_Config [0],drv_Config [1],drv_Config [2],drv_Config [3],drv_Config [4],drv_Config [5],drv_Config [6]);
    return 1;
  } else return 0;
}

/*
 * Starts the DRV. Max 30 seconds. 
 */
bool set_Run_Drv (String ms) {
  start_ms = time_Elapsed; 
  if (ms.toInt()<30000) {duration_ms = ms.toInt(); return 1;}
  else {duration_ms = 0; return 0;}
}


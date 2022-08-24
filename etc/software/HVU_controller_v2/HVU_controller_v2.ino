/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "portmap.h"
#include "HVU.h"

#include <Encoder.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

Encoder myEnc(DIN_ENC_CHANNELB, DIN_ENC_CHANNELA);


#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int hvu_target_voltage = 0;

int hvu_trigger_repeat  = 0;

int hvu_trigger_count   = 0;

int hvu_trigger_delat = 0;

int hvu_current_voltage = 0;

uint8_t hvu_charge_state = 0;

uint16_t setting_delay = 0;

uint16_t count_delay = 0;

uint8_t setting_repeat = 0 ;

uint8_t count_repeat = 0 ;

bool toggle_auto_start = false;

void setup()   
{                
  Serial.begin(9600);

  


  HVU__init();

  MCU__init_pins();

  HVU__enable();

  

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done

  
  showSettings();
  
}


void MCU__init_pins() {
    pinMode( DIN_BTN_START_PAUSE , INPUT_PULLUP );

    pinMode( DIN_ENC_SWITCH , INPUT);
    pinMode( DOUT_ENC_LEDRED , OUTPUT );
    pinMode( DOUT_ENC_LEDGREEN , OUTPUT );
    pinMode( DOUT_ENC_LEDBLUE , OUTPUT );
    digitalWrite( DOUT_ENC_LEDRED , HIGH );
    digitalWrite( DOUT_ENC_LEDGREEN , HIGH );
    digitalWrite( DOUT_ENC_LEDBLUE , HIGH );


    
    pinMode( DOUT_HVU_ENABLE , OUTPUT );
    digitalWrite( DOUT_HVU_ENABLE , HVU_ENABLE );

    pinMode( DOUT_12V_ENABLE , OUTPUT );
    digitalWrite( DOUT_12V_ENABLE , HVU_12V_ENABLE );

    pinMode( DOUT_HVU_TRIGGER , OUTPUT );
    digitalWrite( DOUT_HVU_TRIGGER , HVU_TRIGGER_DISABLE );

    pinMode( AIN_HVU_MEASURE , INPUT );
}


uint8_t menu_mode = 0;
uint8_t menu_mode_old = 0;



void loop() 
{

  static uint16_t nth_counter = 0;
  static uint8_t last_menu = 0;

  uint32_t current_millis = millis();

  HVU__loop();

  if(last_menu != menu_mode)
  {
    last_menu = menu_mode;
    switch(menu_mode)
    {
      case 0:
        myEnc.write((hvu_target_voltage/10)*4);
      break;

      case 1:
        myEnc.write((hvu_target_voltage/10)*4);
      break;

      case 2: // voltage
        myEnc.write((hvu_target_voltage/10)*4);
      break;

      case 3:
        myEnc.write(setting_repeat*4);
      break;

      case 4:
        myEnc.write(setting_delay*4);
      break;
    }

  }

  switch(menu_mode)
  {
    case 0:
      encoder_voltage();

      showStateManual();
    break;

    case 1:
      auto_ignition();

      encoder_voltage();

      showStateAuto();
    break;

    case 2: // voltage
      encoder_voltage();
      showSettings();
    break;

    case 3:
      encoder_repeat();
      showSettings();
    break;

    case 4:
      encoder_delay();
      showSettings();
    break;
  }

  // Serial.println(digitalRead(DIN_ENC_SWITCH));
  // delay(100);

  encoder_btn();

  

  ignition_btn();

  nth_counter++;

  if(nth_counter%4000==0 && HVU__get_state() != HVU__SM_STATE_CHARGED)
  {
    get_current_voltage();

    /*
    HVU__logout_status();

    Serial.print("DOUT_HVU_ENABLE ");
    Serial.println(digitalRead( DOUT_HVU_ENABLE));

    Serial.print("DOUT_12V_ENABLE ");
    Serial.println(digitalRead( DOUT_12V_ENABLE));

    Serial.print("DOUT_HVU_TRIGGER ");
    Serial.println(digitalRead( DOUT_HVU_TRIGGER));
    */

  }

  if(nth_counter%100==0)
  {
    status_led();
  }
  
}


void status_led()
{
  static uint16_t blink_counter = 0;
  static bool r_led = true;
  static bool g_led = true;
  static bool b_led = true;

  static bool blink = false;

  switch(menu_mode)
  {
    case 0:
      r_led = g_led = b_led = true;
      blink = false;

      if(HVU__get_state() == HVU__SM_STATE_CHARGING)
      {
        r_led = false;
        g_led = false;
        blink = true;
      }

      if(HVU__get_state() == HVU__SM_STATE_CHARGED)
      {
        r_led = false;
        blink = true;
      }
    break;

    case 1:
      r_led = g_led = b_led = true;
      blink = false;
      
      if(HVU__get_state() == HVU__SM_STATE_IGNITION_REQUESTED)
      {
        r_led = false;
        blink = true;
      }
    break;

    case 2:
      r_led = g_led = b_led = false;
    break;

    case 3:
      r_led = g_led = b_led = false;
    break;

    case 4:
      r_led = g_led = b_led = false;
    break;

  }

  if(blink)
  {
    blink_counter++;

    if(blink_counter%20==0) {
      digitalWrite( DOUT_ENC_LEDRED , true );
      digitalWrite( DOUT_ENC_LEDGREEN , true );
      digitalWrite( DOUT_ENC_LEDBLUE , true );  
    }

    if(blink_counter%20==5) {
      digitalWrite( DOUT_ENC_LEDRED , r_led );
      digitalWrite( DOUT_ENC_LEDGREEN , g_led );
      digitalWrite( DOUT_ENC_LEDBLUE , b_led );  
    }  
  } else {
    digitalWrite( DOUT_ENC_LEDRED , r_led );
    digitalWrite( DOUT_ENC_LEDGREEN , g_led );
    digitalWrite( DOUT_ENC_LEDBLUE , b_led );  
  }
  
  
}

void get_current_voltage()
{
  hvu_current_voltage = HVU__get_charge();

}

void ignition_btn()
{
  static uint8_t btn_last = 0;
  uint8_t btn = digitalRead(DIN_BTN_START_PAUSE);


  if( btn_last==1 && btn==0 )
  {
    switch(menu_mode) {
      case 0:
        if(HVU__get_state()==HVU__SM_STATE_CHARGED)
        {
          HVU__ignite_at_charg(hvu_target_voltage);
        } else {
          digitalWrite( DOUT_HVU_ENABLE , HVU_ENABLE );
          digitalWrite( DOUT_12V_ENABLE , HVU_12V_ENABLE );
          HVU__start_charging(hvu_target_voltage);  
        }
        
      break;

      case 1:
        toggle_auto_ignition();
      break;
    }
    
    // HVU__start_charging(hvu_target_voltage);  
  }

  btn_last = btn;
  
}

void encoder_btn() 
{
  static uint8_t last_btn_state = 0;
  uint8_t btn_state = digitalRead(DIN_ENC_SWITCH);
  if( last_btn_state==1 && btn_state==0 )
  {
    
    // menu_mode > 3 ? menu_mode = 0 : menu_mode++;

  }
  last_btn_state = btn_state;
}



void encoder_voltage()
{
  long current_encoder = myEnc.read(); 
  if(current_encoder != long(hvu_target_voltage))
  { 
    current_encoder>=0 ? hvu_target_voltage = int(current_encoder/4)*10 : hvu_target_voltage = 0;

    // showSetMenu("voltage",String(hvu_target_voltage) );
  }
}


void encoder_delay()
{
  long current_encoder = myEnc.read(); 
  if(current_encoder != long(setting_delay))
  { 
    current_encoder>=0 ? setting_delay = int(current_encoder/4)*10 : setting_delay = 0;

    // showSetMenu("delay",String(hvu_target_voltage) );
  }
}


void encoder_repeat()
{
  long current_encoder = myEnc.read(); 
  if(current_encoder != long(setting_repeat))
  { 
    current_encoder>=0 ? setting_repeat = int(current_encoder/4) : setting_repeat = 0;

    // showSetMenu("repeat",String(hvu_target_voltage) );
  }
}



void showSetMenu(String label, String value)
{
   display.clearDisplay();
  
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(label);
  
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,9);
  display.println(value);
  // display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.display(); 
}


void showSettings()
{
  display.clearDisplay();
  
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("target V");
  display.setCursor(75,0);
  menu_mode == 2 ? display.println(">") : display.println("=");
  display.setCursor(85,0);
  display.println(hvu_target_voltage);
  

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,9);
  display.println("repeat");
  display.setCursor(75,9);
  menu_mode == 3 ? display.println(">") : display.println("=");
  display.setCursor(85,9);
  display.println(setting_repeat);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,18);
  display.println("repeat delay");
  display.setCursor(75,18);
  menu_mode == 4 ? display.println(">") : display.println("=");
  display.setCursor(85,18);
  display.println(setting_delay);

  // display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.display(); 
}


void showStateManual() 
{
  static uint16_t nthDisplay = 0;

  bool standby = true;

  nthDisplay++;

  if(nthDisplay%1000==0 && HVU__get_state()) {
    display.clearDisplay();
    
    // text display tests
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Manual");
    
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,9);
    display.println(hvu_current_voltage);
    display.setCursor(50,9);
    display.println(">");
    display.setCursor(60,9);
    display.println(hvu_target_voltage);
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,18);

    

    switch(HVU__get_state())
    {
      case HVU__SM_STATE_STANDBY: // standby
        
        

        if(HVU__get_last_ignition()==HVU__STATUS_CODE_IGNITION_SUCCESSFUL)
        {
          display.print("success");
          display.setCursor(50,18);
          display.print(HVU__get_ignition_before_voltage());
          standby = false;
        } 

        if(HVU__get_last_ignition()>=0xE0)
        {
          display.println("failed");
          standby = false;
        } 

        if(standby) {
          display.println("standby");  
        }
      break;

      case HVU__SM_STATE_CHARGING: // standby
        display.println("charging");
      break;

      case HVU__SM_STATE_CHARGED: // hold
        display.println("hold");
      break;

      case HVU__SM_STATE_IGNITION_REQUESTED: // charge
        display.println("wait ignition");
      break;

      default:
      break;
    }
    // display.setTextColor(BLACK, WHITE); // 'inverted' text
    display.display(); 
  }
   
}

uint32_t start_millis;

void showStateAuto()
{
  display.clearDisplay();
  
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Auto");

  display.setCursor(30,0);
  display.println(count_repeat);

  display.setCursor(36+(6*(count_repeat/10)),0);
  display.println("/");

  display.setCursor(42+(6*(count_repeat/10)),0);
  display.println(setting_repeat);

  if(toggle_auto_start && HVU__get_state()==HVU__SM_STATE_STANDBY)
  {
    uint32_t countdown = start_millis-millis();

    display.setCursor(120-(6*(countdown/10)),0);
    display.print(countdown);
  }

  
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,9);
  display.println(hvu_current_voltage);
  display.setCursor(50,9);
  display.println(">");
  display.setCursor(60,9);
  display.println(hvu_target_voltage);
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,18);

  if(toggle_auto_start)
  {
    switch(HVU__get_state())
    {
      case HVU__SM_STATE_STANDBY: // standby
        display.println("wait next");
      break;

      case HVU__SM_STATE_CHARGED: // hold
        display.println("hold");
      break;

      case HVU__SM_STATE_IGNITION_REQUESTED: // charge
        display.println("charge");
      break;

      default:
      break;
    }

  } else {
    display.println("stand by");
  }
  

  
  // display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.display();  
}




void auto_ignition()
{
  static uint8_t laststate = 0x00;
  if(toggle_auto_start)
  {
    if(HVU__get_state()==HVU__SM_STATE_STANDBY)
    {

      if(laststate!=HVU__get_state())
      {
        Serial.println("sdlkjdsfkjdfkjdjk");
        start_millis = millis()+((uint32_t)setting_delay*1000UL);
        laststate = HVU__get_state();
      }

      if(start_millis<=millis())
      {
        Serial.println("ding");
        digitalWrite( DOUT_HVU_ENABLE , HVU_ENABLE );
        digitalWrite( DOUT_12V_ENABLE , HVU_12V_ENABLE );
        HVU__ignite_at_charg(hvu_target_voltage);
        count_delay = 0;
        count_repeat++;
        laststate = 0x00;
      }

      if(count_repeat==setting_repeat)
      {
        toggle_auto_start = false;
      }
    }
  }
}


void toggle_auto_ignition()
{
  toggle_auto_start = !toggle_auto_start;

  if(toggle_auto_start)
  {
    digitalWrite( DOUT_HVU_ENABLE , HVU_ENABLE );
    digitalWrite( DOUT_12V_ENABLE , HVU_12V_ENABLE );
    HVU__ignite_at_charg(hvu_target_voltage);
    count_delay = 0;
    count_repeat = 1;
  }
}

/***********************************************************************
* FILENAME :        exciss_v4_power_supply_evaluation.ino            DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
* Power managment testing and avaluation software.
* Follwoing hardware components are controlled by this library:
* - Charger Texas Instruments bq25700A
* - Battery pack manager and gauge from Texas Instruments BQ40Z50-R2
*
* For more information and documentation go to https://github.com/exciss-space/EXCISS
* AUTHOR :    Shintaro Fujita
*
**/

#include "atmega_port_mapping.h"

#include "exciss.h"

#include "BusMaster.h"

#include "BQ25700A.h"

#include "BQ40Z50.h"


BusMaster busMaster;

BQ25700A charger(BQ25700A_DEFAULT_ADDR);

BQ40Z50 gauge(BQ40Z50_DEFAULT_ADDR);


void setup() {

  TCCR5B = TCCR5B & 0b11111000 | 0x01; // 0x01 = 31250Hz/1, 0x02 = 31250Hz/8

  //***********************//
  // initilize BusMaster   //
  //***********************//
  busMaster = BusMaster();
  busMaster.initBus();
  // busMaster.MuxDevicesArray = MuxDevicesArray;  // assign muxport=device-address lookup table  

  
  Serial.begin(19200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
  
  // put your setup code here, to run once:
  pinMode(2, INPUT); // sniff EV2400 clock   ->  2 [atmel](muss auf input gestellt werden)
  pinMode(3, INPUT); // sniff EV2400 data    ->  3 [atmel](muss auf input gestellt werden)

  bool chargerOk = charger.init();

  bool gaugeOk = gauge.init();

  // charger.enableADC();

  // charger.setImputCurrent(850);

  // charger.setChargeCurrent(320);
  


  pinMode( DOUT_SCU_PWR_ENABLE , OUTPUT );
  digitalWrite( DOUT_SCU_PWR_ENABLE , STATE_SCU_PWR_OFF );

  pinMode( DOUT_SCU_SHUTDOWN_REQUEST , OUTPUT );
  digitalWrite( DOUT_SCU_SHUTDOWN_REQUEST , STATE_SCU_RUN );

  

  pinMode( DOUT_VSYS_ENABLE, OUTPUT );
  digitalWrite( DOUT_VSYS_ENABLE , STATE_VSYS_ON );

  pinMode( DOUT_5V_ENABLE , OUTPUT );
  digitalWrite( DOUT_5V_ENABLE , STATE_5V_ON );

  pinMode( DOUT_12V_ENABLE , OUTPUT );
  digitalWrite( DOUT_12V_ENABLE , STATE_12V_ON );


  pinMode( DOUT_SCU_SD_TOGGLE , OUTPUT );
  digitalWrite( DOUT_SCU_SD_TOGGLE , STATE_SCU_SD_TOGGLE_XU1100 );


  pinMode( DOUT_USBHUB_RESET , STATE_USBHUB_NOT_RESET );
  digitalWrite( DOUT_USBHUB_RESET , LOW); 


  pinMode( DOUT_USBHUB_PORT_1 , OUTPUT );
  pinMode( DOUT_USBHUB_PORT_2 , OUTPUT );


  pinMode( DOUT_HVU_ENABLE , OUTPUT );
  digitalWrite( DOUT_HVU_ENABLE , STATE_HVU_DISABLE );

  pinMode( DOUT_HVU_TRIGGER , OUTPUT );
  digitalWrite( DOUT_HVU_TRIGGER , STATE_HVU_TRIGGER_DISABLE );

  pinMode( AIN_HVU_MEASURE , OUTPUT );
  digitalWrite( AIN_HVU_MEASURE , HIGH );
  
  pinMode( DIN_PSU_CHRG_OK , INPUT);

  pinMode( PWMOUT_FRONTLIGHT_CONTROL , OUTPUT);
  analogWrite( PWMOUT_FRONTLIGHT_CONTROL , 2);

  pinMode( PWMOUT_BACKLIGHT_CONTROL , OUTPUT);
  analogWrite( PWMOUT_BACKLIGHT_CONTROL , 2);


  pinMode( DOUT_USBHUB_PWR , OUTPUT );
  digitalWrite( DOUT_USBHUB_PWR , STATE_USBHUB_PWR_ON );

  pinMode( DOUT_USB_X_PWR , OUTPUT );
  digitalWrite( DOUT_USB_X_PWR , STATE_USB_ON );

  pinMode( DOUT_USB_A_PWR , OUTPUT );
  digitalWrite( DOUT_USB_A_PWR , STATE_USB_ON );

  pinMode( DOUT_USB_B_PWR , OUTPUT );
  digitalWrite( DOUT_USB_B_PWR , STATE_USB_ON );


  pinMode( DOUT_USB_X_TOGGLE , OUTPUT );
  digitalWrite( DOUT_USB_X_TOGGLE , STATE_USB_X_TO_SCU );


  pinMode( DOUT_SCU_PWR_ENABLE , OUTPUT );
  digitalWrite( DOUT_SCU_PWR_ENABLE , STATE_SCU_PWR_ON );




}


void loop()
{

  /*

  Serial.println("//// charger");

  charger.setImputCurrent(850);

  charger.setChargeCurrent(320);

  charger.updateValues();

  Serial.print("adcVBUS ");
  Serial.println(charger.adcVBUS);

  Serial.print("adcPSYS ");
  Serial.println(charger.adcPSYS);

  Serial.print("adcIBATCharge ");
  Serial.println(charger.adcIBATCharge);

  Serial.print("adcIBATDischarge ");
  Serial.println(charger.adcIBATDischarge);

  Serial.print("adcIIN ");
  Serial.println(charger.adcIIN);

  Serial.print("adcCMPIN ");
  Serial.println(charger.adcCMPIN);

  Serial.print("adcVSYS ");
  Serial.println(charger.adcVSYS);

  Serial.print("adcVBAT ");
  Serial.println(charger.adcVBAT);

  */

  // gauge readout

  Serial.println("//// gauge");
  gauge.updateValues();

  /*
  
  Serial.print("maxError ");
  Serial.println(gauge.maxError);

  Serial.print("voltage ");
  Serial.println(gauge.voltage);

  Serial.print("current ");
  Serial.println(gauge.current);

  Serial.print("remainingCurrent ");
  Serial.println(gauge.remainingCurrent);

  Serial.print("fullChargCapacity "); 
  Serial.println(gauge.fullChargCapacity);

  Serial.print("minToFull "); 
  Serial.println(gauge.minToFull);

  Serial.print("minToEmpty "); 
  Serial.println(gauge.minToEmpty);

  Serial.print("minRuntime ");
  Serial.println(gauge.minRuntime);

  Serial.print("temperature "); 
  Serial.println(gauge.temperature);

  Serial.print("chargeRelative ");
  Serial.println(gauge.chargeRelative);

  Serial.print("chargeAbsolute ");
  Serial.println(gauge.chargeAbsolute);

  */

  Serial.print("batteryStatus ");
  Serial.println(gauge.batteryStatus,BIN); // see github issue https://github.com/exciss-space/EXCISS/issues/3

  /*

  Serial.print("cycleCount "); 
  Serial.println(gauge.cycleCount);

  Serial.print("cell1voltage "); 
  Serial.println(gauge.cell1voltage);

  Serial.print("cell2voltage "); 
  Serial.println(gauge.cell2voltage);
  
  Serial.print("cell3voltage "); 
  Serial.println(gauge.cell3voltage);

  Serial.print("cell4voltage ");
  Serial.println(gauge.cell4voltage);
  */

  delay(10000);
 
}
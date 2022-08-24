/***********************************************************************
* FILENAME :        portmap.h             DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
*       Portmapping and port value definition config file.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    18 Feb 2018
*
**/

#ifndef _CONFIG_ATMEGA_PORTS_H_
#define _CONFIG_ATMEGA_PORTS_H_

// ----------------------------------------
// ----------------------------------------
// POWER INTERCONNECTIONS
#define DOUT_VSYS_ENABLE      30
#define DOUT_3V_ENABLE        31
#define DOUT_12V_ENABLE       32
#define DOUT_SCU_PWR_ENABLE   24
#define DIN_PSU_CHRG_OK       2

#define STATE_VSYS_ON         HIGH
#define STATE_VSYS_OFF        LOW

#define STATE_3V_ON           HIGH
#define STATE_3V_OFF          LOW

#define STATE_12V_ON          HIGH
#define STATE_12V_OFF         LOW

#define STATE_SCU_PWR_ON      HIGH
#define STATE_SCU_PWR_OFF     LOW



// ----------------------------------------
// ----------------------------------------
// USB POWER INTERCONNECTIONS
#define DOUT_USB_X_PWR      A8
#define DOUT_USB_B_PWR      A9
#define DOUT_USB_A_PWR      A10

// State definitions
#define STATE_USB_ON        HIGH
#define STATE_USB_OFF       LOW


// ----------------------------------------
// ----------------------------------------
// USB X SWITCH
#define DOUT_USB_X_TOGGLE       5

// State definitions
#define STATE_USB_X_TO_ISS      LOW
#define STATE_USB_X_TO_SCU      HIGH


// ----------------------------------------
// ----------------------------------------
// USBHUB INTERCONNECTIONS
#define DOUT_USBHUB_PORT_1              A11 // !change require usbhub reset
#define DOUT_USBHUB_RESET               A12 // HIZ or LOW
#define DOUT_USBHUB_PORT_2              A13 // !change require usbhub reset
#define DOUT_USBHUB_PWR                 A14

// State definitions
#define STATE_USBHUB_RESET              OUTPUT
#define STATE_USBHUB_NOT_RESET          INPUT

#define STATE_USBHUB_PWR_ON             HIGH
#define STATE_USBHUB_PWR_OFF            LOW


// ----------------------------------------
// ----------------------------------------
// SCU INTERCONNECTIONS

#define DOUT_SCU_SHUTDOWN_REQUEST     25
#define DOUT_SCU_SD_TOGGLE            26 // LOW = XU1100, HIGH = XU1200

// State definitions
#define STATE_SCU_SD_TOGGLE_XU1100    LOW
#define STATE_SCU_SD_TOGGLE_XU1200    HIGH

#define STATE_SCU_SHUTDOWN_REQUEST    HIGH
#define STATE_SCU_RUN                 LOW


// ----------------------------------------
// ----------------------------------------
// HVU INTERCONNECTIONS
#define DOUT_HVU_ENABLE             17
#define DOUT_HVU_TRIGGER            16
#define AIN_HVU_MEASURE             A0


#define STATE_HVU_ENABLE            HIGH
#define STATE_HVU_DISABLE           LOW

#define STATE_HVU_TRIGGER_ENABLE    HIGH
#define STATE_HVU_TRIGGER_DISABLE   LOW


// ----------------------------------------
// ----------------------------------------
// I2CMUX INTERCONNECTIONS
#define DOUT_I2CMUX_RST             38
#define DOUT_I2CMUX_SDA             20
#define DOUT_I2CMUX_SCL             21

#define STATE_BUSMASTER_RESET_DO_RESET 	LOW
#define STATE_BUSMASTER_RESET_NORMAL 	HIGH


// ----------------------------------------
// ----------------------------------------
// CHAMBER LIGHTING
#define PWMOUT_FRONTLIGHT_CONTROL   44
#define PWMOUT_BACKLIGHT_CONTROL    45


// ----------------------------------------
// ----------------------------------------
// MOTOR DRIVER
#define DOUT_MOT_ENABLE             33

#define STATE_MOT_ENABLE            HIGH
#define STATE_MOT_DISABLE           LOW


// ----------------------------------------
// ----------------------------------------
// EEPROM
#define DOUT_EEPROM_HOLD            6
#define DOUT_EEPROM_CS              12
#define DOUT_EEPROM_WP              10
#define DOUT_EEPROM_MISO            50
#define DOUT_EEPROM_MOSI            51
#define DOUT_EEPROM_SCK             52


// ----------------------------------------
// ----------------------------------------
// RTC
#define DIN_RTC_INT                 19


#endif
// ------------------------------------
// powermenegment
#define POWERMANAGER_CELL_CAPACITY 2200
#define POWERMANAGER_EXPERIMENT_MIN_CHARGE 50 // value unit is percent
#define POWERMANAGER_SMALLTHINGS_MIN_CHARGE 20 // value unit is percent



// ------------------------------
// pinout mapping revision 00B

// left side
#define CORE__PIN_DIN_RASPI_WATCHDOG						3
#define CORE__PIN_DOUT_MOSFET_5V							4
#define CORE__PIN_DOUT_MOSFET_8V							5
#define CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER		7
#define CORE__PIN_DOUT_SCIENCE_ARC_CHARG					8
#define CORE__PIN_PWM_POWERLED_FRONT						9

// right side
#define CORE__PIN_AIN_SCIENCE_ARC_CHARG_VOLTAGE				A2
#define CORE__PIN_AIN_USB_POWER_SENSE						A0
#define CORE__PIN_DOUT_USB_SWITCH_SWITCH					12
#define CORE__PIN_DOUT_BABYSITTER_SYSOFF					11
#define CORE__PIN_PWM_POWERLED_BACK							10


// ------------------------------------
// UART config
#define UART_BAUTRATE	9600


// ------------------------------------
// usb switch definitions
#define USBSWITCH__SWITCH_TRANSFER	LOW
#define USBSWITCH__SWITCH_RASPI		HIGH


// ------------------------------------
// init state machine value definitions
#define CORE__INIT_EXECUTE	0xA5A5A5A5
#define CORE__INIT_DONE		0x5A5A5A5A

#define CORE__INIT_STATE_SET_PWM_FREQ						0x11111111
#define CORE__INIT_STATE_I2C_BABYSITTER						0x22222222
#define CORE__INIT_STATE_I2C_RTC							0x33333333
#define CORE__INIT_STATE_INIT_SERIAL						0x44444444
#define CORE__INIT_STATE_END								0xFFFFFFFF


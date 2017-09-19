// ------------------------------
// pinout mapping revision 00A

// left side
#define CORE__PIN_DIN_RASPI_WATCHDOG						3
#define CORE__PIN_DOUT_MOSFET_5V							4
#define CORE__PIN_DOUT_MOSFET_8V							5
#define CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER		7
#define CORE__PIN_DOUT_SCIENCE_ARC_CHARG					8
#define CORE__PIN_PWM_POWERLED_FRONT						9

// right side
#define CORE__PIN_AIN_SCIENCE_ARC_CHARG_VOLTAGE				A2
#define CORE__PIN_DOUT_USB_SWITCH_SWITCH					A0
#define CORE__PIN_DOUT_BABYSITTER_SYSOFF					13
#define CORE__PIN_DIN_BABYSITTER_PGOOD						12
#define CORE__PIN_DIN_BABYSITTER_GPOUT						11
#define CORE__PIN_PWM_POWERLED_BACK							10


// ------------------------------------
// UART config
#define UART_BAUTRATE	9600


// ------------------------------------
// init state machine value definitions
#define CORE__INIT_EXECUTE	0xA5A5A5A5
#define CORE__INIT_DONE		0x5A5A5A5A

#define CORE__INIT_STATE_SET_PWM_FREQ						0x11111111
#define CORE__INIT_STATE_SET_I2C_COMPONENT_TO_INIT_STATE	0x22222222
#define CORE__INIT_STATE_INIT_SERIAL						0x33333333
#define CORE__INIT_STATE_END								0xFFFFFFFF


// ------------------------------------
#define USBSWITCH__SWITCH_TRANSFER	LOW
#define USBSWITCH__SWITCH_RASPI		HIGH
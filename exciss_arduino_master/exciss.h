// ------------------------------------
// timekeeper

// configs
#define CORE__DATATRANSFER_WINDOW_START_HOUR 		18
#define CORE__DATATRANSFER_WINDOW_END_HOUR 			20
#define CORE__DATATRANSFER_WINDOW_MARGIN_MINUTE 	10

#define TIMEKEEPER__DATA_TRANSFER_WINDOW			0xA5
#define TIMEKEEPER__SCIENCE_WINDOW					0x5A



// ------------------------------------
// powermenegment
#define POWERMANAGER_CELL_CAPACITY 2200
#define POWERMANAGER_EXPERIMENT_MIN_CHARGE 50 // value unit is percent
#define POWERMANAGER_SMALLTHINGS_MIN_CHARGE 20 // value unit is percent

#define POWERMANAGER_CALL_FREQUENCY 1 // call period (in seconds)

#define POWERMANAGER_N_POWERDOWN 3
#define POWERMANAGER_N_RELOADCONFIG 4
#define POWERMANAGER_N_TIMEREF 2

#define POWERMANAGER_STATUSBIT_HAS_POWERDOWN 0
#define POWERMANAGER_STATUSBIT_HAS_TIMEREF 1
#define POWERMANAGER_STATUSBIT_HAS_RELOADCONFIG 2

#define POWERMANAGER_THERSHOLD_MIN_USB_ON 100
#define POWERMANAGER_THERSHOLD_MAX_USB_OFF 50

// times in milliseconds
//const uint32_t POWERMANAGER_T_OFF_MIN=50*((uint32_t)1000); // minimum length of power low cycle
//const uint32_t POWERMANAGER_T_OFF_MAX=70*((uint32_t)1000); // maximum length of power low cycle
//const uint32_t POWERMANAGER_T_WAIT_MAX=5*60*((uint32_t)1000); // maximum distance of power low cycles
const uint32_t POWERMANAGER_T_OFF_MIN=4*((uint32_t)1000); // minimum length of power low cycle
const uint32_t POWERMANAGER_T_OFF_MAX=8*((uint32_t)1000); // maximum length of power low cycle
const uint32_t POWERMANAGER_T_WAIT_MAX=5*6*((uint32_t)1000); // maximum distance of power low cycles




// ------------------------------
// pinout mapping revision 00C

// left side
#define CORE__PIN_DIN_RASPI_WATCHDOG						3
#define CORE__PIN_DOUT_MOSFET_5V							4
#define CORE__PIN_DOUT_MOSFET_8V							5
#define CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER		7
#define CORE__PIN_DOUT_SCIENCE_ARC_CHARG					8
#define CORE__PIN_PWM_POWERLED_FRONT						9

// right side
#define CORE__PIN_AIN_SCIENCE_ARC_CHARG_VOLTAGE				A2
#define CORE__PIN_DOUT_USB_SWITCH_SWITCH					13
#define CORE__PIN_DOUT_BABYSITTER_PGOOD						12
#define CORE__PIN_DOUT_BABYSITTER_SYSOFF					11
#define CORE__PIN_PWM_POWERLED_BACK							10


// ------------------------------------
// UART config
#define UART_BAUTRATE	9600


// ------------------------------------
// usb switch config
#define USBSWITCH__SWITCH_TRANSFER		LOW
#define USBSWITCH__SWITCH_SCIENCE		HIGH


// ------------------------------------
// ignition config
#define CORE_IGNITION_MAX_CHARGTIME_MILLIS			4000000
#define CORE_IGNITION_RETRY_MAX_COUNT				5
#define CORE_IGNITION_RETRY_DELAY_MILLIS			300
#define CORE_IGNITION_MIN_CHARG_VOLTAGE				100
#define CORE_IGNITION_MAX_CHARG_VOLTAGE				300

// analog input pin for charge monitoring
#define CHARGEMONITOR_INPUT_PIN A0

// pin to enable arc capacitor charging
#define CHARGEMONITOR_CHARGE_ENABLE_PIN 10
#define CHARGEMONITOR_CHARGE_ENABLE HIGH
#define CHARGEMONITOR_CHARGE_DISABLE LOW

// pin to toggle for ignition
#define CHARGEMONITOR_IGNITE_PIN 11
#define CHARGEMONITOR_IGNITE_IGNITE HIGH
#define CHARGEMONITOR_IGNITE_DISABLE LOW
#define CHARGEMONITOR_IGNITE_TOGGLE_DURATION 10 // milliseconds

// duration of spark
#define CHARGEMONITOR_IGNITION_WAIT 100 // milliseconds

// voltage threshold for determining whether ignition was successful (capacitor discharged to ca. 70V) - set above physical threshold
#define CHARGEMONITOR_EMPTY_THRESHOLD 100
#define CHARGEMONITOR_VOLTAGE_SCALE 400/3 // beware of integer divisions!


// ------------------------------------
// raspberry pi operation state
#define RASPBERRY__state_offline		0;
#define RASPBERRY__state_booting		1;
#define RASPBERRY__state_operational	2;
#define RASPBERRY__state_shutdown		4;


// ------------------------------------
// init state machine value definitions
#define CORE__INIT_EXECUTE	0xA5A5A5A5
#define CORE__INIT_DONE		0x5A5A5A5A

#define CORE__INIT_STATE_SET_PWM_FREQ						0x11111111
#define CORE__INIT_STATE_I2C_BABYSITTER						0x22222222
#define CORE__INIT_STATE_I2C_RTC							0x33333333
#define CORE__INIT_STATE_INIT_SERIAL						0x44444444
#define CORE__INIT_STATE_END								0xFFFFFFFF


// ------------------------------------
// main state machine definitions
#define CORE__MAIN_SM_T_RECOVER								101
#define CORE__MAIN_SM_L_RECOVER								102
#define CORE__MAIN_SM_T_IDLE								201
#define CORE__MAIN_SM_L_IDLE								202
#define CORE__MAIN_SM_T_DATATRANSFER_MODE					301
#define CORE__MAIN_SM_L_DATATRANSFER_MODE 					302
#define CORE__MAIN_SM_T_DATATRANSFER_MODE_CLOSE_DELAY		303
#define CORE__MAIN_SM_T_SCIENCE_GO_CONDITION_CHECK			401
#define CORE__MAIN_SM_T_SCIENCE_GO							402
#define CORE__MAIN_SM_T_SCIENCE_GO_DELAY					403
#define CORE__MAIN_SM_T_SCIENCE_GO_RASPI_POWERUP			404
#define CORE__MAIN_SM_L_SCIEMCE_WAIT_RASPI_BOOT				405
#define CORE__MAIN_SM_T_SCIENCE_RASPI_BOOT_DONE				406
#define CORE__MAIN_SM_L_SCIENCE_MISSION_ABORT_OR_DONE		407
#define CORE__MAIN_SM_T_SCIENCE_ABORT						408
#define CORE__MAIN_SM_T_SCIENCE_STOP						409
#define CORE__MAIN_SM_T_SCIENCE_SHUTDOWN					410


// ------------------------------------
// power managment state machine definitions
#define CORE__POWER_SM_T_INIT								101
#define CORE__POWER_SM_T_HIBERNATE_MODE						201
#define CORE__POWER_SM_L_HIBERNATE_MODE						202
#define CORE__POWER_SM_T_SLEEP_MODE							301
#define CORE__POWER_SM_L_SLEEP_MODE							302
#define CORE__POWER_SM_T_SCIENCE_MODE						401
#define CORE__POWER_SM_L_SCIENCE_POWER_READY				402
#define CORE__POWER_SM_T_SCIENCE_POWER_OK					403
#define CORE__POWER_SM_L_SCIENCE_POWER_MODE					404


// ------------------------------------
// ignition managment state machine definitions
#define CORE__IGNITION_SM_T_INIT								101
#define CORE__IGNITION_SM_L_OFF									102
#define CORE__IGNITION_SM_L_IDLE								201
#define CORE__IGNITION_SM_T_CHARGE								301
#define CORE__IGNITION_SM_L_CHARGE								302
#define CORE__IGNITION_SM_T_IGNITION_READY						401
#define CORE__IGNITION_SM_L_IGNITION_WAIT						402
#define CORE__IGNITION_SM_T_IGNITION_IGNITE						403
#define CORE__IGNITION_SM_L_IGNITION_REDO						404
#define CORE__IGNITION_SM_T_IGNITION_REDO_DELAY					405
#define CORE__IGNITION_SM_T_IGNITION_DONE						406
#define CORE__IGNITION_SM_T_ABORT_DUE_FAILURE					901





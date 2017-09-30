// ------------------------------------
// timekeeper

// configs
#define CORE__DATATRANSFER_WINDOW_START_HOUR        19
#define CORE__DATATRANSFER_WINDOW_END_HOUR          21

#define TIMEKEEPER__DATA_TRANSFER_WINDOW            11
#define TIMEKEEPER__SCIENCE_WINDOW                  22



// ------------------------------------
// powermenegment
#define POWERMANAGER_CELL_CAPACITY 3400*4
#define POWERMANAGER_EXPERIMENT_MIN_CHARGE 50 // value unit is percent
#define POWERMANAGER_SMALLTHINGS_MIN_CHARGE 20 // value unit is percent

// #define POWERMANAGER_CALL_FREQUENCY 1 // call period (in seconds)

#define POWERMANAGER_N_RELOADCONFIG             4

#define POWERMANAGER_POWERCYCLE_WINDOW_MILLIS   1000UL*60UL
#define POWERMANAGER_POWERCYCLE_MIN_INTERVAL    2000UL

#define POWERMANAGER_STATUSBIT_HAS_POWERDOWN 0
#define POWERMANAGER_STATUSBIT_HAS_TIMEREF 1
#define POWERMANAGER_STATUSBIT_HAS_RELOADCONFIG 2


#define POWERMANAGER_FORCE_POWER_DOWN_WAIT_MILLIS        1000UL*60UL*30UL




// ------------------------------
// pinout mapping revision 00E

// left side
#define CORE__PIN_DOUT_MOSFET_5V                            4
#define CORE__PIN_DOUT_MOSFET_8V                            5
#define CORE__PIN_DOUT_FORCE_RASPI_SHUTDOWN                 6
#define CORE__PIN_DOUT_SCIENCE_IGNITION_SPARK_TRIGGER       7
#define CORE__PIN_DOUT_SCIENCE_ARC_CHARG                    8
#define CORE__PIN_PWM_POWERLED_FRONT                        9

// right side
#define CORE__PIN_AIN_SCIENCE_ARC_CHARG_VOLTAGE             A2
#define CORE__PIN_DOUT_USB_SWITCH_SWITCH                    13
#define CORE__PIN_DOUT_BABYSITTER_PGOOD                     12
#define CORE__PIN_DOUT_BABYSITTER_SYSOFF                    11
#define CORE__PIN_PWM_POWERLED_BACK                         10

// etc.


// ------------------------------------
// UART config
#define UART_BAUTRATE   9600


// ------------------------------------
// usb switch config
#define USBSWITCH__SWITCH_TRANSFER      LOW
#define USBSWITCH__SWITCH_SCIENCE       HIGH


// ------------------------------------
// ignition config
#define CORE_IGNITION_MAX_CHARGTIME_MILLIS          1000UL*300UL
#define CORE_IGNITION_RETRY_MAX_COUNT               5
#define CORE_IGNITION_RETRY_DELAY_MILLIS            300
#define CORE_IGNITION_MIN_CHARG_VOLTAGE             100
#define CORE_IGNITION_MAX_CHARG_VOLTAGE             300

// pin to enable arc capacitor charging
#define CHARGEMONITOR_CHARGE_ENABLE HIGH
#define CHARGEMONITOR_CHARGE_DISABLE LOW

// pin to toggle for ignition
#define CHARGEMONITOR_IGNITE_IGNITE HIGH
#define CHARGEMONITOR_IGNITE_DISABLE LOW
#define CHARGEMONITOR_IGNITE_TOGGLE_DURATION 10 // milliseconds

// duration of spark
#define CHARGEMONITOR_IGNITION_WAIT 100 // milliseconds

// voltage threshold for determining whether ignition was successful (capacitor discharged to ca. 70V) - set above physical threshold
#define CHARGEMONITOR_EMPTY_THRESHOLD 100
#define CHARGEMONITOR_VOLTAGE_SCALE 1 // 400/3 // beware of integer divisions!


// ------------------------------------
// science operations
#define SCIENCE__DEFAULT_KEEPALIVE_TIME_MILLIS      60000UL*5UL
#define SCIENCE__DEFAULT_WAKEUP_TIMER_MILLIS        60000UL*30UL
#define SCIENCE__DEFAULT_WAIT_UNTIL_POWERDOWN       1000UL*30UL



// ------------------------------------
// Core values
#define CORE__OPERATION_MODE_RECOVERY           0xA5
#define CORE__OPERATION_MODE_SCIENCE            0x5A

// ------------------------------------
// init state machine value definitions
#define CORE__INIT_EXECUTE  0xA5A5A5A5
#define CORE__INIT_DONE     0x5A5A5A5A

#define CORE__INIT_STATE_I2C_BABYSITTER                     1101
#define CORE__INIT_STATE_I2C_RTC                            1102
#define CORE__INIT_STATE_END                                1900


// ------------------------------------
// main state machine definitions

// states
#define CORE__MAIN_SM_DELAY                                 2000
#define CORE__MAIN_SM_L_RECOVERMODE_DATATRANSFER_MODE       2001
#define CORE__MAIN_SM_L_IDLE                                2101
#define CORE__MAIN_SM_T_SCIENCE_GO                          2201
#define CORE__MAIN_SM_T_SCIENCE_GO_DELAY                    2202
#define CORE__MAIN_SM_T_SCIENCE_GO_RASPI_POWERUP            2203
#define CORE__MAIN_SM_L_SCIENCE_RASPI_KEEPALIVE             2204
#define CORE__MAIN_SM_T_SCIENCE_POWERDOWN_DELAY             2205

// execution delays
#define CORE__MAIN_SM_DELAY_EXIT_RECOVERYMODE_MILLIS        1000UL*60UL*60UL*60UL

// ------------------------------------
// power managment state machine definitions

// states
#define CORE__POWER_SM_DELAY                                3000
#define CORE__POWER_SM_T_INIT                               3101
#define CORE__POWER_SM_L_IDLE_MODE                          3201
#define CORE__POWER_SM_ENTER_RECOVERY_MODE_DATATRANSFER     3202
#define CORE__POWER_SM_L_RECOVERY_MODE_START_SCU            3301

// execution delays
#define CORE__POWER_SM_DELAY_ENTER_RECOVERYMODE_MILLIS      1000UL*60UL*5UL
// #define CORE__POWER_SM_DELAY_ENTER_SCU_POWERON_MILLIS       1000UL*60UL*30UL // flight version
#define CORE__POWER_SM_DELAY_ENTER_SCU_POWERON_MILLIS       1000UL*10UL // dev version



// miscellaneous
// #define CORE__POWER_MIN_KEEPALIVE_TIME_MILLIS               1000UL*60UL*5UL // flight version
#define CORE__POWER_MIN_KEEPALIVE_TIME_MILLIS               1000UL*60UL*30UL  // dev version

// ------------------------------------
// ignition managment state machine definitions

// states
#define CORE__IGNITION_SM_L_OFF                                 4101
#define CORE__IGNITION_SM_L_IDLE                                4201
#define CORE__IGNITION_SM_L_CHARGE                              4301
#define CORE__IGNITION_SM_T_IGNITION_READY                      4401
#define CORE__IGNITION_SM_T_IGNITION_IGNITE                     4403
#define CORE__IGNITION_SM_L_IGNITION_REDO                       4404
#define CORE__IGNITION_SM_T_IGNITION_REDO_DELAY                 4405
#define CORE__IGNITION_SM_T_ABORT_DUE_FAILURE                   4901


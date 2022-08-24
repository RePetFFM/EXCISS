/***********************************************************************
* DESCRIPTION :
*       main arduino code for EXCISS version 4, PCB revision 1.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    29 Jun 2018
*
**/

#ifndef _CONFIG_HVU_H_
#define _CONFIG_HVU_H_

#define HVU__IGNITE_TOGGLE_DURATION         10

#define HVU__MEASURE_WAIT_AFTER_IGNITION    10

#define HVU__DISCHARGE_SUCCESS_THRESHOLD    100

#define HVU__MIN_CHARGE_VALUE               10

#define HVU__MAX_CHARGE_VALUE               550

#define HVU__CAPACITOR_MAX_VOLTAGE_VALUE    550

// ------------------------------------
// ignition config

#define HVU__CHARGING_MAX_TIME_UNTIL_ABORT_SEC                          1200UL

#define HVU__CHARGED_MAX_WAIT_TIME_IGNITION_REQUEST_UNTIL_ABORT_SEC     1200UL

#define HVU__IGNITION_MAX_WAIT_TIME_IGNITION_CHARG_UNTIL_ABORT_SEC      1200UL

#define HVU__IGNITION_MAX_CHARGTIME_MILLIS                              1000UL*300UL

#define HVU__IGNITION_RETRY_MAX_COUNT                                   5

#define HVU__IGNITION_RETRY_DELAY_MILLIS                                500

#define HVU__IGNITION_DELTA                                             1


// ------------------------------------
// test ignition conditions
#define HVU__TEST_IGNITION_CONDITION_MAX_CAP_VOLTAGE					30


// duration of spark
// #define CHARGEMONITOR_IGNITION_WAIT 100 // milliseconds

// voltage threshold for determining whether ignition was successful (capacitor discharged to ca. 70V) - set above physical threshold
// #define CHARGEMONITOR_EMPTY_THRESHOLD 150


// ------------------------------------
// ignition managment state machine definitions

// ----------------------------------------
// ----------------------------------------
// HVU STATEMACHINE STATE DEFINITIONS
#define HVU__SM_STATE_OFF                   0xA5

#define HVU__SM_STATE_STANDBY               0xB0

#define HVU__SM_STATE_CHARGING              0xA0

#define HVU__SM_STATE_CHARGED               0xAD

#define HVU__SM_STATE_IGNITION_REQUESTED    0x65

#define HVU__SM_STATE_IGNITE                0x5A

#define HVU__SM_STATE_ABORT                 0xAA

#define HVU__SM_STATE_ABORT_WITH_ERROR      0xFF

// states
// #define CORE__IGNITION_DELAY                                    4000
// #define CORE__IGNITION_SM_L_OFF                                 4101
// #define CORE__IGNITION_SM_L_IDLE                                4201
// #define CORE__IGNITION_SM_L_CHARGE                              4301
#define HVU__IGNITION_SM_T_IGNITION_READY                   4401
// #define CORE__IGNITION_SM_T_IGNITION_IGNITE                     4403
#define HVU__IGNITION_SM_L_IGNITION_REDO                    4404
// #define CORE__IGNITION_SM_T_ABORT_DUE_FAILURE                   4901

// ----------------------------------------
// ----------------------------------------
// HVU STATUS CODES
#define HVU__STATUS_CODE_IGNITION_WAIT                            0x00

#define HVU__STATUS_CODE_IGNITION_SUCCESSFUL                      0xAA

#define HVU__STATUS_CODE_ERROR_CHARG_TIMEOUT                      0xE1

#define HVU__STATUS_CODE_ERROR_CHARG_OVERSHOOTED                  0xE2

#define HVU__STATUS_CODE_ERROR_CHARG_HOLD_TIMEOUT                 0xE3

#define HVU__STATUS_CODE_ERROR_CHARG_HOLD_OVERSHOOTED             0xE4

#define HVU__STATUS_CODE_ERROR_IGNITION_CHARG_TIMEOUT             0xE5

#define HVU__STATUS_CODE_ERROR_IGNITION_OVERSHOOTED               0xE6

#define HVU__STATUS_CODE_ERROR_IGNITION_FAILED_THRESHOLD          0xE7


#endif
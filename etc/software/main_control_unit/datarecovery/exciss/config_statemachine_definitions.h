
#ifndef _CONFIG_SM_H_
#define _CONFIG_SM_H_


// ----------------------------------------
// ----------------------------------------
// MCU STATEMACHINE STATE DEFINITIONS
#define SM_MCU__STATE_INIT          0xA5
#define SM_MCU__STATE_HEALTHCHECK   0xB3
#define SM_MCU__STATE_NORMAL        0x5A
#define SM_MCU__STATE_PTC_LOCK		0xF1


// ----------------------------------------
// ----------------------------------------
// SCU STATEMACHINE STATE DEFINITIONS
#define SM_SCU__OPERATION_MODE_RECOVERY   0xA5
#define SM_SCU__OPERATION_MODE_NORMAL     0x5A


#define SM_SCIENCE__INIT                        0x50
#define SM_SCIENCE__OFF                         0x5A
#define SM_SCIENCE__SCU_BOOTUP_CONDITION_CHECK  0xA0
#define SM_SCIENCE__SCU_BOOTUP                  0xA1
#define SM_SCIENCE__SCU_RUNNING                 0xA2
#define SM_SCIENCE__SCU_SHUTDOWN                0xA3
#define SM_SCIENCE__SCU_SHUTDOWN_COMPLETE       0xA4


#endif
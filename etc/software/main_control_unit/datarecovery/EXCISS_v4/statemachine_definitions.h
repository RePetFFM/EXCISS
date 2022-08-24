
// ----------------------------------------
// ----------------------------------------
// MCU STATEMACHINE STATE DEFINITIONS
#define SM_MCU__STATE_INIT		0xA5
#define SM_MCU__STATE_NORMAL	0x5A


// ----------------------------------------
// ----------------------------------------
// SCU STATEMACHINE STATE DEFINITIONS
#define SM_SCU__OPERATION_MODE_RECOVERY   0xA5
#define SM_SCU__OPERATION_MODE_NORMAL     0x5A


// ----------------------------------------
// ----------------------------------------
// HVU STATEMACHINE STATE DEFINITIONS
#define SM_HVU__STATE_OFF				0xA5

#define SM_HVU__STATE_IDLE				0xBD

#define SM_HVU__STATE_CHARGING			0xDB
#define SM_HVU__STATE_CHARGED			0xBD
#define SM_HVU__STATE_CHARGE_FAIL		0x5D

#define SM_HVU__STATE_IGNITE			0x5A
#define SM_HVU__STATE_IGNITION_FAIL		0xFF



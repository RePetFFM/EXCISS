// ----------------------------------------
// ----------------------------------------
// POWER INTERCONNECTIONS
#define DOUT_5V_ENABLE  	A14
#define DOUT_12V_ENABLE  	A13


// ----------------------------------------
// ----------------------------------------
// USB POWER INTERCONNECTIONS
#define DOUT_USB_X_PWR  A8
#define DOUT_USB_B_PWR  A9
#define DOUT_USB_A_PWR  A10

// State definitions
#define USB_ON      HIGH
#define USB_ONFF    LOW


// ----------------------------------------
// ----------------------------------------
// USB X SWITCH
#define DOUT_USB_X_TOGGLE  5

// State definitions
#define USB_X_TO_ISS    LOW
#define USB_X_TO_SCU    HIGH


// ----------------------------------------
// ----------------------------------------
// USBHUB INTERCONNECTIONS
#define DOUT_USBHUB_PORT_1  A11 // !change require usbhub reset
#define DOUT_USBHUB_RESET   A12 // HIZ or LOW
#define DOUT_USBHUB_PORT_2  A13 // !change require usbhub reset
#define DOUT_USBHUB_PWR     A14

// State definitions
#define USBHUB_RESET       	OUTPUT
#define USBHUB_NOT_RESET  	INPUT

#define USBHUB_PWR_ON   	HIGH
#define USBHUB_PWR_OFF  	LOW


// ----------------------------------------
// ----------------------------------------
// SCU INTERCONNECTIONS
#define DOUT_SCU_PWR                24
#define DOUT_SCU_SHUTDOWN_REQUEST   25
#define DOUT_SCU_SD_TOGGLE          26 // LOW = XU1100, HIGH = XU1200

// State definitions
#define SCU_PWR_ON              HIGH
#define SCU_PWR_OFF             LOW

#define SCU_SD_TOGGLE_XU1100    LOW
#define SCU_SD_TOGGLE_XU1200    HIGH


// ----------------------------------------
// ----------------------------------------
// HVU INTERCONNECTIONS
#define DOUT_HVU_ENABLE     17
#define DOUT_HVU_TRIGGER    16


// ----------------------------------------
// ----------------------------------------
// I2CMUX INTERCONNECTIONS
#define DOUT_I2CMUX_RST     38
#define DOUT_I2CMUX_SDA     20
#define DOUT_I2CMUX_SCL     21


// ----------------------------------------
// ----------------------------------------
// CHAMBER LIGHTING
#define PWMOUT_FRONTLIGHT_CONTROL   44
#define PWMOUT_BACKLIGHT_CONTROL    45




// ----------------------------------------
// ----------------------------------------
// STATEMACHINE STATE DEFINITIONS
#define SCU__OPERATION_MODE_RECOVERY   0xA5
#define SCU__OPERATION_MODE_NORMAL     0x5A


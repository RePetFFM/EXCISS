#define POWERMANAGER_PIN_PGOOD 10
#define POWERMANAGER_PIN_SYSOFF 11

// call period (in seconds)
#define POWERMANAGER_CALL_FREQUENCY 1

#define POWERMANAGER_N_POWERDOWN 3
#define POWERMANAGER_N_RELOADCONFIG 4
#define POWERMANAGER_N_TIMEREF 2

// times in milliseconds
//const uint32_t POWERMANAGER_T_OFF_MIN=50*((uint32_t)1000); // minimum length of power low cycle
//const uint32_t POWERMANAGER_T_OFF_MAX=70*((uint32_t)1000); // maximum length of power low cycle
//const uint32_t POWERMANAGER_T_WAIT_MAX=5*60*((uint32_t)1000); // maximum distance of power low cycles
const uint32_t POWERMANAGER_T_OFF_MIN=4*((uint32_t)1000); // minimum length of power low cycle
const uint32_t POWERMANAGER_T_OFF_MAX=8*((uint32_t)1000); // maximum length of power low cycle
const uint32_t POWERMANAGER_T_WAIT_MAX=5*6*((uint32_t)1000); // maximum distance of power low cycles

#define POWERMANAGER_CELL_CAPACITY 2200
#define POWERMANAGER_EXPERIMENT_MIN_CHARGE 50 // percent
#define POWERMANAGER_SMALLTHINGS_MIN_CHARGE 20 // percent

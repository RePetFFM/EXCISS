#include <Arduino.h>
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


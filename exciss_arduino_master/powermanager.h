
bool powermanager_begin();
void powermanager_poll();

// power-cycle command requests
uint8_t powermanager_has_command_powerdown();
uint8_t powermanager_has_new_reference_time();
uint8_t powermanager_has_command_reload_config();

// misc
uint32_t powermanager_current_time(); // in seconds since reference time

// power state
uint8_t powermanager_has_power_for_experiment(); // minimum battery level and power good
uint8_t powermanager_has_power_for_small_things(); // minimum battery level

// temp
uint16_t powermanager_get_charge_state();

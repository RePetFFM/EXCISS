
uint8_t powermanager_begin();
void powermanager_poll_powercycle_command();

// power-cycle command requests
uint8_t powermanager_has_command_powerdown();
uint8_t powermanager_has_new_reference_time();
uint8_t powermanager_has_command_reload_config();

// misc
uint32_t get_last_powercylce_command_timestamp();

// power state
uint16_t powermanager_get_voltage();
uint16_t powermanager_get_capacity();
uint8_t powermanager_get_usb_power_status();
uint8_t powermanager_has_power_for_experiment(); // minimum battery level and power good
uint8_t powermanager_has_power_for_small_things(); // minimum battery level

uint8_t powermanager_shutdown_requested();
// temp
uint16_t powermanager_get_charge_state();


// power switching
void powermanager_11V_on();
void powermanager_11V_off();
void powermanager_5V_on();
void powermanager_5V_off();

void powermanager_vsys_on();
void powermanager_vsys_off();
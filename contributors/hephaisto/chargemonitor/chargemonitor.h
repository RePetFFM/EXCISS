extern int chargemonitor_last_target_voltage;
extern int chargemonitor_last_actual_voltage;
extern int chargemonitor_last_after_ignition_voltage;

void chargemonitor_begin();
void chargemonitor_start_charging(int voltage);
void chargemonitor_abort();
bool chargemonitor_has_enough_charge();
void chargemonitor_ignite();
bool chargemonitor_last_ignition_succeeded();
int chargemonitor_get_charge();

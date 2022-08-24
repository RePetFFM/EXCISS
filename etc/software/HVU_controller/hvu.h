extern int HVU__last_target_voltage;
extern int HVU__last_actual_voltage;
extern int HVU__last_after_ignition_voltage;
extern int HVU__requested_ignition_voltage = 0;

void 	HVU__begin();
void 	HVU__start_charging(int voltage);

void 	HVU__stop_charging();
void 	HVU__abort();
bool 	HVU__has_enough_charge();
void 	HVU__ignite();
bool 	HVU__last_ignition_succeeded();
int 	HVU__get_charge();


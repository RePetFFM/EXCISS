/***********************************************************************
* FILENAME :        hvu.h             DESIGN REF: EXCISS_v4
*
* DESCRIPTION :
*       library for high voltage unit periferial control.
*
* 
* AUTHOR :    Shintaro Fujita        START DATE :    18 Feb 2018
*
**/


#ifndef _SCU_H_
#define _SCU_H_

#define SCU__BOOT_MODE_RECOVERY		0x55
#define SCU__BOOT_MODE_SCIENCE		0xAA



void SCU__init();

void SCU__loop();

void SCU__set_next_bootup_mode(uint32_t a_boot_mode);

void SCU__request_bootup();

void SCU__state_bootup();

void SCU__request_shutdown();

void SCU__logout_current_mode();

void SCU__sd_card_to_XU1100();

void SCU__sd_card_to_XU1200();

void SCU__logout_sd_card_active();

void SCU__bootdown_finalise();

void SCU__sd_card_switching();



#endif // _SCU_H_
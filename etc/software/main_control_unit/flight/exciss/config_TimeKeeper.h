/***********************************************************************
* DESCRIPTION :
*
* 
* AUTHOR :    Shintaro Fujita
*
**/


#ifndef _CONFIG_TIMEKEEPER_H_
#define _CONFIG_TIMEKEEPER_H_

// ------------------------------------
// timekeeper

// configs
#define TIMEKEEPER__DATATRANSFER_WINDOW_START_HOUR        			19    // 19

#define TIMEKEEPER__DATATRANSFER_WINDOW_END_HOUR          			21    // 21

#define TIMEKEEPER__DATATRANSFER_WINDOW_START_MARGIN_SECONDS      	30UL * 60UL    // 30 minutes

// ------------------------------------
// timer event ID


#define TIMEKEEPER__TIMEREVENT_TEMPSENS_UPDATE              0x51

// ------------------------------------
// timer event array

#define TIMEKEEPER__TIMEREVENT_ARRAY_COUNT                              1

#define TIMEKEEPER__TIMEREVENT_ARRAY_INDEX_TEMPSENS_UPDATE				0

#endif
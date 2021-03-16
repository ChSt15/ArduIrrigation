#ifndef _SYSTEM_H_
#define _SYSTEM_H_



#include <Arduino.h>

#include "utils/dataStructs.h"
#include "utils/network/network.h"
#include "utils/sleep/sleep.h"
#include "utils/userInterface/userInterface.h"

#include "EEPROM/EEPROM.h"
#include "comms/CommsLib.h"
#include "IOControl/IOControl.h"
#include "sensors/sensors.h"

#include "Wire.h"
#include "RTClib.h"


extern SystemStatus systemStatusGlobal;
extern ValveTiming valveTiming[8];
extern RTC_DS3231 rtc;


extern uint8_t latestNodeStatus;
extern int8_t nodeStatusRSSI[MAX_NUM_NODES];
extern int8_t nodeStatusSNR[MAX_NUM_NODES];
extern SystemStatus nodeStatus[MAX_NUM_NODES];
extern uint32_t nodeStatusCounter[MAX_NUM_NODES];

extern uint8_t latestNodeTime;
extern int8_t nodeTimeRSSI[MAX_NUM_NODES];
extern int8_t nodeTimeSNR[MAX_NUM_NODES];
extern DateTime nodeTime[MAX_NUM_NODES];
extern uint32_t nodeTimeCounter[MAX_NUM_NODES];



void progInit();
void progLoop(void);

void printSystemData();

void userSettings(); //THIS SHOULD ONLY BE ADDED WHEN WRITING TO RTC AND OR EEPROM TO SAVE NEW SETTINGS WHILE TESTING


void networkHandling();

void networkTest();






#endif
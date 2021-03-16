#ifndef _EEPROM_H_
#define _EEPROM_H_



#include <Arduino.h>

#include "utils/dataStructs.h"

#include <EEPROM.h>
#include "AT24C.hpp"
#include "RTClib.h"



#define EEPROM_INT_SIZE 500
#define EEPROM_EXT_SIZE 4000

#define EEPROM_VERSION 2
#define EEPROM_DATA_START_ADDRESS 10

#define EEPROM_VERSION_MARK EEPROM_VERSION + EEPROM_DATA_START_ADDRESS //this will be saved into EEPROM to make sure nothing bad happends and avoid user error



static AT24C eepromExt;

struct SaveSystemData {
    uint16_t softwareVersion = 0;
    uint8_t nodeID = 255;
    float vBatCorrectionFactor = 1.0f;
    DateTime now;
    uint32_t valveOnTime[8] = {0}; // time in minutes of the valves
    POWERSTATUS systemPower = POWERSTATUS::POWERSAVING;
};

static SaveSystemData saveSystemData;



void initEEPROM();

void saveSystemStatus(SystemStatus status);
void saveWateringTiming(SystemStatus status);

uint16_t getEEPROMBootCount();
void setEEPROMBootCount(uint32_t bootCount);

bool loadSystemStatus(SystemStatus *status);
bool loadWateringTiming(ValveTiming *timing);






#endif
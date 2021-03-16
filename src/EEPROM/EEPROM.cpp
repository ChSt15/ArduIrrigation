#include "EEPROM/EEPROM.h"

#include <Arduino.h>




void initEEPROM() {

    EEPROM.begin(EEPROM_INT_SIZE);

}



void saveSystemStatus(SystemStatus status) {

    saveSystemData.softwareVersion = status.softwareVersion;
    saveSystemData.nodeID = status.nodeID;
    saveSystemData.vBatCorrectionFactor = status.vBatCorrectionFactor;
    saveSystemData.now = status.now;
    
    for (int i = 0; i < 8; i++) saveSystemData.valveOnTime[i] = status.valveOnTime[i];

    saveSystemData.systemPower = status.systemPower;

    EEPROM.write(0, EEPROM_VERSION_MARK);

    EEPROM.writeBytes(EEPROM_DATA_START_ADDRESS, (uint8_t*)&saveSystemData, sizeof(SaveSystemData));

    EEPROM.commit();

}



void saveWateringTiming(ValveTiming* timing) {

    eepromExt.write(0, EEPROM_VERSION_MARK);

    eepromExt.write(EEPROM_DATA_START_ADDRESS, (uint8_t*)timing, sizeof(timing));

}



bool loadSystemStatus(SystemStatus *status) {

    uint8_t eepromVersion = EEPROM.read(0);

    if (eepromVersion == EEPROM_VERSION_MARK) { // Check if versions match

        EEPROM.readBytes(EEPROM_DATA_START_ADDRESS, (uint8_t*)&saveSystemData, sizeof(SaveSystemData));

        status->softwareVersion = saveSystemData.softwareVersion;
        status->nodeID = saveSystemData.nodeID;
        status->vBatCorrectionFactor = saveSystemData.vBatCorrectionFactor;
        status->now = saveSystemData.now;
        
        for (int i = 0; i < 8; i++) status->valveOnTime[i] = saveSystemData.valveOnTime[i];

        status->systemPower = saveSystemData.systemPower;

        return true;

    }

    return false;

}



bool loadWateringTiming(ValveTiming *timing) {

    uint8_t eepromVersion = eepromExt.read(0);

    if (eepromVersion == EEPROM_VERSION_MARK) {

        eepromExt.read(EEPROM_DATA_START_ADDRESS, (uint8_t*)timing, sizeof(timing));

        return true;

    }

    return false;

}



uint16_t getEEPROMBootCount() {
    return EEPROM.readULong(450);
}



void setEEPROMBootCount(uint32_t bootCount) {
    EEPROM.writeULong(450, bootCount);
    EEPROM.commit();
}
#ifndef _DATASTRUCTS_H_
#define _DATASTRUCTS_H_

#include <Arduino.h>

#include "definitions.h"

#include "RTClib.h"


enum POWERSTATUS {
    POWERNORMAL,
    POWERSAVING,
    POWERSHUTDOWN
};



enum DEVICESTATUS {
    RUNNING,
    NOT_RUNNING,
    FAILURE
};



/*class ValveTime {
public:

    uint8_t hour = 0;
    uint16_t minute = 0;
    uint8_t length = 0; // how long to open valve. If 0 then inactive
    
    
    uint16_t time () { return (uint16_t) minute + hour*60; }

    void normalize() {


        while (minute >= 60) {
            minute -= 60;
            hour++;
        }

        hour = hour%24;

    }

    bool operator > (ValveTime &right) { return this->time() > right.time(); }

    bool operator < (ValveTime &right) { return this->time() < right.time(); }

    bool operator == (ValveTime &right) { return this->time() == right.time(); }

    bool operator >= (ValveTime &right) { return this->time() >= right.time(); }

    bool operator <= (ValveTime &right) { return this->time() <= right.time(); }

    bool operator != (ValveTime &right) { return !(*this == right); }

    ValveTime operator + (ValveTime &right) { 

        ValveTime time;

        time.hour = hour + right.hour;

        time.minute = minute + right.minute;

        time.normalize();

        return time;

    }

};*/



struct ValveTiming {

    uint8_t hour;
    uint8_t minute;
    uint16_t length; // how long to open valve. If 0 then inactive

    uint8_t valve = 255;

};



struct SystemStatus {

    uint16_t softwareVersion = 0;

    uint8_t nodeID = 255;

    bool wifiEnabled = false;

    float vBatCorrectionFactor = 1.0f;

    DateTime now;
    DateTime systemStartTime;

    //ValveTiming valveTiming[8];

    uint32_t valveOnTime[8] = {0}; // time in minutes of the valves

    float airTemperature = -1.0f;
    float airPressure = -1.0f;
    float airHumidity = -1.0f;
    float soilMoisture[2] = {-1.0f, -1.0f};

    float vBat = 0.0f;

    uint8_t uiOpenCount = 0;
    uint32_t bootCount = 0;

    POWERSTATUS systemPower = POWERSTATUS::POWERSAVING;

    DEVICESTATUS airSensorStatus = DEVICESTATUS::NOT_RUNNING;
    DEVICESTATUS soilSensorStatus = DEVICESTATUS::NOT_RUNNING;
    DEVICESTATUS interfaceStatus = DEVICESTATUS::NOT_RUNNING;
    DEVICESTATUS rtcStatus = DEVICESTATUS::NOT_RUNNING;
    DEVICESTATUS eepromStatus = DEVICESTATUS::NOT_RUNNING;
    DEVICESTATUS ioStatus = DEVICESTATUS::NOT_RUNNING;
    DEVICESTATUS rgbLEDStatus = DEVICESTATUS::NOT_RUNNING;
    DEVICESTATUS networkStatus = DEVICESTATUS::NOT_RUNNING;

};




#endif
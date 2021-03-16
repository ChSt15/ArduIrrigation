#ifndef _OUTPUTCONTROL_H_
#define _OUTPUTCONTROL_H_

#include <Arduino.h>

#include "definitions.h"

#include "utils/QueueData/queueData.h"

#include "MCP23S17.h"



#define VALVE_CHANGE_TIME 500

#define ADC_NUM_MEASUREMENTS 10

#define IOVALVE_CLOSE false
#define IOVALVE_OPEN true


//#define BoardVersionV1_0 //REMOVE AFTER TESTING



namespace IOControl {


    void begin();

    bool setValveState(byte valve, bool state);
    void setVBatFactor(float factor);

    float getBatteryVoltage();
    bool getValveState(uint8_t valve);
    float getVBatFactor();

    uint16_t _getADC(int pin, bool extPower);

    void loop();

}



#endif
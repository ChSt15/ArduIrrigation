#ifndef _USERINTERFACE_H_
#define _USERINTERFACE_H_

#include <Arduino.h>

#include "SSD1306Wire.h"  

#include "utils/dataStructs.h"
#include "IOControl/IOControl.h"
#include "waterControl/waterControl.h"
#include "definitions.h"

#include "EEPROM/EEPROM.h"

#include "system/system.hpp"


#define PIN_READ_RATE_MS 10
#define PIN_AVG_LPF_FACTOR 0.9999f
#define PIN_AVG_LPF_FACTOR_OFF 0.9995f
#define PIN_ACTIVE_THRESHOLD 18.0f
#define INTERFACE_UPDATE_RATE 25

#define LONG_PRESS_THRESHOLD 500 
#define INTERFACE_FRONT_TIMEOUT 5000
#define INTERFACE_MAIN_TIMEOUT 30000

#define FRONT_CODE_COUNT_NUM 3

#define TOUCH_A 0
#define TOUCH_B 1
#define TOUCH_C 2



namespace UserInterface {
	
    bool begin();

    void loop(SystemStatus _systemStatus);

    bool interfaceActivity();
    bool getTouch(uint8_t pin);
    uint8_t touchThreshold(uint8_t pin);

};






#endif
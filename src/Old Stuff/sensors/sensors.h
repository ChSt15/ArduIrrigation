#ifndef _SENSORS_H_
#define _SENSORS_H_

#include <Arduino.h>
#include "Wire.h"

#include "definitions.h"
#include "IOControl/IOControl.h"
#include "system/system.hpp"

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define AIRDATA_MEASUREMENT_INTERVAL 100 



namespace Sensors {


    void begin();

    float getHumidity();
    float getPressure();
    float getTemperature();

    void enableGroundMoistureSensor(uint8_t sensorNum, bool enable = true);

    void loop();

}



#endif
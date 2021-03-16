#include <Arduino.h>
#include <Wire.h>


#include "system/system.hpp"
//#include "EEPROM/EEPROM.h"
#include "utils/arrayBuffer/arrayBuffer.h"


#include "definitions.h"



void setup() {

    progInit();

}



void loop() {

    progLoop();

}





/*void sleepForTime(uint32_t sleepLength) {

    esp_sleep_enable_timer_wakeup((uint64_t)sleepLength*1000);

    //Serial.println("Sleep Start!");

    delay(10);

    esp_light_sleep_start();

    //Serial.println("Wake Up!");

    

}



void sleepWithInt(uint32_t sleeplength) {

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 1);
    sleepForTime(sleeplength);

}*/

#include "ArduIrrigationBase/Timing/rtc_ds3231.h"



Clock_DS3231::Clock_DS3231(): Task_Threading("DS3231 Clock", eTaskPriority_t::eTaskPriority_Middle, 1*MINUTES) {

    

}




void Clock_DS3231::setTime(int64_t time) {

    DS3231 clock;
    clock.setEpoch(time/SECONDS);

}



void Clock_DS3231::thread() {

    int64_t timeLocal = NOW();

    int64_t time = int64_t(RTClib::now().unixtime())*SECONDS;

    clockTopic_.publish(DataTimestamped<int64_t>(time, timeLocal));

}


void Clock_DS3231::init() {

    Wire.begin();

    thread();

}


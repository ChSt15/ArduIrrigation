#include "ArduIrrigationBase/Timing/watering_dailytime.h"

#include "ArduIrrigationBase/Timing/watering_scheduler.h"



Watering_DailyTime::Watering_DailyTime(Valve_Abstract& valve, int hour, int minute, int64_t length): Task_Threading("Watering Daily", eTaskPriority_t::eTaskPriority_High), valve_(valve) {
    
    wateringStart_ = (int64_t) hour*HOURS + minute*MINUTES;
    length_ = length;

}


void Watering_DailyTime::thread() {

    WateringScheduler::addWateringTime(length_, valve_);

    TimeDate sysTime = SystemTime::getCurrentDateTime();

    TimeDate timeTrig = TimeDate(sysTime.year(), sysTime.month(), sysTime.day()) + TimeSpan(wateringStart_);

    if (sysTime > timeTrig) timeTrig = timeTrig + 1*DAYS;

    nextWateringTime_ = timeTrig;
    
    suspendUntil(timeTrig.nanosecondsTime() - SystemTime::getSystemStartDateTime());

}


void Watering_DailyTime::init() {

    TimeDate sysTime = SystemTime::getCurrentDateTime();

    TimeDate timeTrig = TimeDate(sysTime.year(), sysTime.month(), sysTime.day()) + TimeSpan(wateringStart_);

    if (sysTime > timeTrig) timeTrig = timeTrig + 1*DAYS;

    nextWateringTime_ = timeTrig;
    
    suspendUntil(timeTrig.nanosecondsTime() - SystemTime::getSystemStartDateTime());

}


TimeDate Watering_DailyTime::getNextWateringTime() const {

    return nextWateringTime_;

}
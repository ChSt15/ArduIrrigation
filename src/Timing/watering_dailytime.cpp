#include "watering_dailytime.h"



Watering_DailyTime::Watering_DailyTime(Valve_Abstract& valve, int hour, int minute, int64_t length): Task_Threading("Watering Daily", eTaskPriority_t::eTaskPriority_High, 1*SECONDS), valve_(valve) {
    
    wateringStart_ = (int64_t) hour*HOURS + minute*MINUTES;
    length_ = length;

}


void Watering_DailyTime::thread() {

    TimeDate sysTime = SystemTime::getCurrentDateTime();

    int64_t time = (int64_t) sysTime.hour()*HOURS + sysTime.minute()*MINUTES + sysTime.second()*SECONDS;

    bool isTime = time > wateringStart_ && time < (wateringStart_ + length_);

    if (isTime && !valve_.isOpen()) valve_.open(true);
    else if (!isTime && valve_.isOpen()) valve_.open(false);

}


void Watering_DailyTime::init() {
    
    valve_.open(false);

}


TimeDate Watering_DailyTime::getNextWateringTime() const {

    TimeDate sysTime = SystemTime::getCurrentDateTime();

    TimeDate timeTrig = TimeDate(sysTime.year(), sysTime.month(), sysTime.day()) + TimeSpan(wateringStart_);

    TimeDate timePoint = timeTrig;

    if (timeTrig < sysTime) timePoint = TimeDate(sysTime.year(), sysTime.month(), sysTime.day()) + TimeSpan(1*DAYS + wateringStart_);

    return timePoint;

}
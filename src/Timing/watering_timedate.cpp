#include "watering_timedate.h"



Watering_TimeDate::Watering_TimeDate(Valve_Abstract& valve, int hour, int minute, int64_t length): Task_Threading("Watering DateTime", eTaskPriority_t::eTaskPriority_High, 1*SECONDS), valve_(valve) {
    
    TimeDate time = SystemTime::getCurrentDateTime();

    wateringStart_ = TimeDate(time.year(), time.month(), time.day()) + TimeSpan(hour*HOURS + minute*MINUTES);
    length_ = length;

}


Watering_TimeDate::Watering_TimeDate(Valve_Abstract& valve, TimeDate timeDate, int64_t length): Task_Threading("Watering DateTime", eTaskPriority_t::eTaskPriority_High, 1*SECONDS), valve_(valve) {

    wateringStart_ = timeDate;
    length_ = length;

}


void Watering_TimeDate::thread() {

    TimeDate sysTime = SystemTime::getCurrentDateTime();

    int64_t time = (int64_t) sysTime.hour()*HOURS + sysTime.minute()*MINUTES + sysTime.second()*SECONDS;

    int64_t waterTime = wateringStart_.nanosecondsTime();

    bool isTime = time > waterTime && time < (waterTime + length_);

    if (isTime && !valve_.isOpen()) valve_.open(true);
    else if (!isTime && valve_.isOpen()) valve_.open(false);

}


void Watering_TimeDate::init() {
    
    valve_.open(false);

}


TimeDate Watering_TimeDate::getNextWateringTime() const {

    return wateringStart_;

}
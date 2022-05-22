#include "ArduIrrigationBase/Timing/Watering_Cycle.h"



Watering_Cycle::Watering_Cycle(Valve_Abstract& valve, int64_t cyclePeriod, int64_t length, int64_t startTime, int64_t endTime): Task_Threading("Watering Cycle", eTaskPriority_t::eTaskPriority_High, cyclePeriod), valve_(valve) {
    
    cyclePeriod_ = cyclePeriod;

    cycleLength_ = length;

    cycleStart_ = startTime;
    cycleEnd_ = endTime;

}


void Watering_Cycle::thread() {

    WateringScheduler::addWateringTime(cycleLength_, valve_);

    TimeDate next = getNextWateringTime();

    suspendUntil(next.nanosecondsTime() - SystemTime::getSystemStartDateTime());

}


void Watering_Cycle::init() {
    
    TimeDate next = getNextWateringTime();

    suspendUntil(next.nanosecondsTime() - SystemTime::getSystemStartDateTime());

}


TimeDate Watering_Cycle::getNextWateringTime() const {

    TimeDate sysTimeDate = SystemTime::getCurrentDateTime();
    int64_t sysTime = sysTimeDate.nanosecondsTime();
    int64_t today = TimeDate(sysTimeDate.year(), sysTimeDate.month(), sysTimeDate.day()).nanosecondsTime();
    int64_t nextTime;

    if (cycleEnd_ < cycleStart_) {

        int64_t refETime = today + cycleEnd_;
        int64_t refSTime = today + cycleStart_;

        if (sysTime > refETime && sysTime < refSTime) {

            nextTime = refSTime; 

        } else if (sysTime > refSTime) {

            int64_t sinceStart = sysTime - refSTime;

            int64_t tillNext = cyclePeriod_ - sinceStart%cyclePeriod_;

            nextTime = sysTime + tillNext;

        } else {

            int64_t sinceStart = sysTime - (refSTime - 1*DAYS);

            int64_t tillNext = cyclePeriod_ - sinceStart%cyclePeriod_;

            nextTime = sysTime + tillNext;

        }


    } else {

        int64_t refETime = today + cycleEnd_;
        int64_t refSTime = today + cycleStart_;

        if (sysTime < refETime && sysTime > refSTime) {

            int64_t sinceStart = sysTime - refSTime;

            int64_t tillNext = cyclePeriod_ - sinceStart%cyclePeriod_;

            nextTime = sysTime + tillNext;

        } else if (sysTime < refSTime) {

            nextTime = refSTime;

        } else {

            nextTime = refSTime + 1*DAYS;

        }

        //nextTime = refSTime;

    }

    return TimeDate(nextTime/SECONDS);

}
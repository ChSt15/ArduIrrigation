#include "Watering_Cycle.h"

Watering_Cycle::Watering_Cycle(Valve_Abstract& valve, int64_t cyclePeriod, int64_t length, int64_t startTime, int64_t endTime): Task_Threading("Watering Event", eTaskPriority_t::eTaskPriority_Realtime, 1*SECONDS), valve_(valve) {
    
    cyclePeriod_ = cyclePeriod;

    cycleLength_ = length;

    cycleStart_ = startTime;
    cycleEnd_ = endTime;
    if (cycleStart_ > cycleEnd_) cycleEnd_ += 24*HOURS;

}


void Watering_Cycle::thread() {

    if (currentValveState_) {

        valve_.open(false);
        currentValveState_ = false;

        TimeDate sysTime = SystemTime::getCurrentDateTime();

        if (sysTime.nanosecondsTime() + cycleLength_ > cycleEnd_ + cycleStartDate_.nanosecondsTime()) cycleStartDate_ = sysTime.today();

        TimeDate nextWaterTime = getNextWateringTime();
        suspendUntil(NOW() + nextWaterTime.nanosecondsTime() - SystemTime::getSyncTime());

    } else if (!currentValveState_) {
        
        valve_.open(true);
        currentValveState_ = true;

        suspendUntil(NOW() + cycleLength_);

    }

}


void Watering_Cycle::init() {
    
    //valve_.open(false);

    TimeDate sysTime = SystemTime::getCurrentDateTime();
    cycleStartDate_ = sysTime.today();

    if (cycleEnd_ > 24*HOURS && (sysTime - cycleStartDate_).nanosecondsTime() < (cycleEnd_ - cycleStart_)) cycleStartDate_ = cycleStartDate_.unixtime() - 1*DAYS/SECONDS;

    TimeDate nextWaterTime = getNextWateringTime();

    suspendUntil(NOW() + nextWaterTime.nanosecondsTime() - SystemTime::getSyncTime());

}


TimeDate Watering_Cycle::getNextWateringTime() const {

    TimeDate sysTime = SystemTime::getCurrentDateTime();

    TimeDate cycleStartDate = cycleStartDate_ + TimeSpan(cycleStart_);
    TimeDate cycleEndDate = cycleStartDate_ + TimeSpan(cycleEnd_);

    if (sysTime < cycleStartDate) return cycleStartDate;

    TimeSpan timeSinceStart = sysTime - cycleStartDate;

    int64_t cycles = timeSinceStart.nanosecondsTime()/cyclePeriod_;
    int64_t nextCycle = (int64_t)(cycles + 1) * cyclePeriod_ + cycleStartDate.nanosecondsTime();

    if (nextCycle + cycleLength_ > cycleEndDate.nanosecondsTime()) return cycleStartDate + TimeSpan(1*DAYS);

    return TimeDate(nextCycle/SECONDS);

}
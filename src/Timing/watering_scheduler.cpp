#include "ArduIrrigationBase/Timing/watering_scheduler.h"



TimeDate WateringScheduler::nextAvailableTime_ = 0;


WateringScheduler::WateringScheduler() {}


void WateringScheduler::addWateringTime(const TimeSpan& length, const ValveGroup& valves) {

    TimeDate startTime = nextAvailableTime_;
    if (startTime < SystemTime::getCurrentDateTime()) startTime = SystemTime::getCurrentDateTime();
    nextAvailableTime_ = startTime + length;

    new WateringEvent(valves, startTime, length); //Will delete itself after finishing watering.

}



WateringScheduler::WateringEvent::WateringEvent(const ValveGroup& valveGroup, const TimeDate& startTime, const TimeSpan& length): Task_Threading("Watering Event", eTaskPriority_t::eTaskPriority_Realtime) {

    startTime_ = startTime;
    length_ = length;

    valveGroup_ = valveGroup;

    suspendUntil(startTime_.nanosecondsTime() - SystemTime::getSystemStartDateTime());

}


void WateringScheduler::WateringEvent::thread() {

    if (!valveGroup_.isOpen()) {

        valveGroup_.setValves(true);

        suspendUntil(NOW() + length_.nanosecondsTime());

    } else {

        valveGroup_.setValves(false);

        delete this; //Kinda stupid dangerous, please only let watering scheduler use the watering event class.

    }

}

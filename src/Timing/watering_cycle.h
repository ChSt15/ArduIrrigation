#ifndef WATERING_CYCLE_H
#define WATERING_CYCLE_H


#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"

#include "KraftKontrol/utils/system_time.h"

#include "../IOControl/Valves/valve_abstract.h"

#include "watering_abstract.h"



class Watering_Cycle: public Watering_Abstract, public Task_Threading {
private:

    Valve_Abstract& valve_;

    TimeDate nextWaterTrigger_ = 0;
    bool currentValveState_ = false;
    TimeDate waterBeginTimestamp_ = 0;

    int64_t cyclePeriod_ = 0;
    int64_t cycleLength_ = 0;

    int64_t cycleStart_ = 0;
    int64_t cycleEnd_ = 24*HOURS;

    TimeDate cycleStartDate_;


public: 

    Watering_Cycle(Valve_Abstract& valve, int64_t cyclePeriod, int64_t length, int64_t startTime, int64_t endTime);


public:

    void thread() override;

    void init() override;

    TimeDate getNextWateringTime() const override;


};


#endif
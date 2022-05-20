#ifndef WATERING_DAILYTIME_H
#define WATERING_DAILYTIME_H


#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"

#include "KraftKontrol/utils/system_time.h"

#include "../IOControl/Valves/valve_abstract.h"

#include "watering_abstract.h"



class Watering_DailyTime: public Watering_Abstract, public Task_Threading {
private:

    Valve_Abstract& valve_;

    int64_t wateringStart_ = 0;
    int64_t length_ = 0;

    TimeDate nextWateringTime_ = 0;

public: 

    /**
     * @brief Construct a new Watering_TimeDate object with hour and minute
     * 
     * @param valve Which valve to control
     * @param hour 
     * @param minute 
     * @param length How long to water for in nanoseconds
     */
    Watering_DailyTime(Valve_Abstract& valve, int hour, int minute, int64_t length);


public:

    void thread() override;

    void init() override;

    TimeDate getNextWateringTime() const override;


};


#endif
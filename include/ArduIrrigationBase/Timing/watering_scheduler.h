#ifndef WATERING_SCHEDULER_H
#define WATERING_SCHEDULER_H


#include "KraftKontrol/utils/system_time.h"
#include "KraftKontrol/utils/date_time.h"
#include "KraftKontrol/utils/list.h"
#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"

#include "valve_group.h"

#include "stdint.h"


class WateringScheduler {
private:

    class WateringEvent: Task_Threading {
    private:

        TimeDate startTime_ = 0;
        TimeSpan length_ = 0;

        ValveGroup valveGroup_;

        bool wasRan_ = false;


    public:

        WateringEvent(const ValveGroup& valveGroup, const TimeDate& startTime, const TimeSpan& length);


    private:

        void thread();

    };

    static TimeDate nextAvailableTime_;


    WateringScheduler(); // Make sure this class wont be instantiated from outside


public:

    /**
     * @brief Adds a request to the scheduler to water for a given time.
     * 
     * @param length How long to water with given valve group.
     * @param valves Group of valves to turn on for watering.
     */
    static void addWateringTime(const TimeSpan& length, const ValveGroup& valves);


};


#endif
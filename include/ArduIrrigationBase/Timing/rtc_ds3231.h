#ifndef RTC_DS3231_H
#define RTC_DS3231_H


#include "KraftKontrol/utils/clock_abstract.h"
#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"

#include "lib/ds3231/DS3231.h"

#include "Arduino.h"


class Clock_DS3231: public Clock_Abstract, public Task_Threading {
private:


public: 

    Clock_DS3231();


public:

    void thread() override;


    void init() override;


    void setTime(int64_t time);


};


#endif
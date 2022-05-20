#ifndef ELECTROMAGNET_VALVE_H
#define ELECTROMAGNET_VALVE_H


#include "KraftKontrol/hal/gpio_hal_abstract.h"
#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"
#include "KraftKontrol/utils/system_time.h"

#include "valve_abstract.h"


/**
 * @brief class to control impulse valves. 
 * 
 */
class Valve_Electromagnet: public Valve_Abstract, public Task_Threading {
private:

    GPIO_HAL_Abstract& outA_;
    GPIO_HAL_Abstract& outB_;

    uint64_t triggerTimestamp_ = 0;

    bool isOpen_ = false;


public:

    /**
     * @param outA Positive GPIO for open.
     * @param outB Negative GPIO for open.
     * @param pulseTime_ns How long to pulse for state change.
     * 
     */
    Valve_Electromagnet(GPIO_HAL_Abstract& outA, GPIO_HAL_Abstract& outB);

    bool isOpen() override;

    void init() override;

    void thread() override;


private: 

    void openValve(bool open) override;


};



#endif
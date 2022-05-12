#ifndef IMPULSE_VALVE_H
#define IMPULSE_VALVE_H


#include "KraftKontrol/hal/gpio_hal_abstract.h"
#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"
#include "KraftKontrol/utils/system_time.h"

#include "valve_abstract.h"


/**
 * @brief class to control impulse valves. 
 * 
 */
class Valve_Impulse: public Valve_Abstract, public Task_Threading {
private:

    int64_t pulseTime_;

    GPIO_HAL_Abstract& outA_;
    GPIO_HAL_Abstract& outB_;

    uint64_t triggerTimestamp_ = 0;

    bool isOpen_ = false;

    bool stateCommand_ = false;

    bool firstRun_ = true;

    static Valve_Impulse* valveChange_;


public:

    /**
     * @param outA Positive GPIO for open.
     * @param outB Negative GPIO for open.
     * @param pulseTime_ns How long to pulse for state change.
     * 
     */
    Valve_Impulse(GPIO_HAL_Abstract& outA, GPIO_HAL_Abstract& outB, int64_t pulseTime_ns = 200*MILLISECONDS);

    bool isOpen() override;

    void init() override;

    void thread() override;


private: 

    void openValve(bool open) override;


};



#endif
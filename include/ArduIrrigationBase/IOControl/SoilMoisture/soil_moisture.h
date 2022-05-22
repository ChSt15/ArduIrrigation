#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H


#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"
#include "KraftKontrol/utils/system_time.h"

#include "Arduino.h"


/**
 * @brief class to control impulse valves. 
 * 
 */
class SoilMoisture: public Task_Threading {
private:

    float moisturePercent_ = 0;

    uint32_t pin_ = 0;

public:

    SoilMoisture(uint32_t pin, float updateRate);

    void init() override;

    void thread() override;

    /**
     * @returns soil moisture in percent 
     */
    float getMoisturePercent();


};



#endif
#ifndef VALVE_ABSTRACT_H
#define VALVE_ABSTRACT_H


#include "KraftKontrol/utils/list.h"


/**
 * @brief abstract class to control valves. 
 * 
 */
class Valve_Abstract {
private:

    static bool valveShutLock_;

    static List<Valve_Abstract*>& globalValvesList_();

public:

    Valve_Abstract();

    virtual ~Valve_Abstract();

    /**
     * @brief Forces all valves to close even if locked
     * 
     */
    static void forceAllValvesShut(bool forceShut);

    /**
     * @brief If all valves state should be locked.
     * 
     * @param lock 
     */
    static void lockAllValveState(bool lock);

    /**
     * @brief If all valve states are locked shut.
     * 
     * @return true 
     * @return false 
     */
    static bool getlockAllValveState();

    /**
     * @brief Sets the valve state.
     * 
     * @param open Opens valve if true, closes otherwise.
     */
    void open(bool open);

    /**
     * @brief Returns the current valve state.
     * 
     * @return true 
     * @return false 
     */
    virtual bool isOpen() = 0;


private:

    virtual void openValve(bool open) = 0;

};



#endif
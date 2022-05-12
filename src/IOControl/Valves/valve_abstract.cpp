#include "valve_abstract.h"


bool Valve_Abstract::valveShutLock_ = false;


Valve_Abstract::Valve_Abstract() {
    globalValvesList_().append(this);
}


Valve_Abstract::~Valve_Abstract() {
    globalValvesList_().removeAllEqual(this);
}


List<Valve_Abstract*>& Valve_Abstract::globalValvesList_() {

    static List<Valve_Abstract*> globalValvesList;

    return globalValvesList;

}


void Valve_Abstract::forceAllValvesShut(bool forceShut) {

    if (forceShut) {

        for (uint32_t i = 0; i < globalValvesList_().getNumItems(); i++) globalValvesList_()[i]->openValve(false);

    }

}


bool Valve_Abstract::getlockAllValveState() {

    return valveShutLock_;

}


void Valve_Abstract::lockAllValveState(bool lock) {

    valveShutLock_ = lock;

}


void Valve_Abstract::open(bool open) {

    if (!valveShutLock_) openValve(open);

}


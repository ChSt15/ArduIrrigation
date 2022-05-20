#include "ArduIrrigationBase/Timing/valve_group.h"


ValveGroup::ValveGroup() {}


ValveGroup::ValveGroup(Valve_Abstract& valve) {
    addValveToGroup(valve);
}


void ValveGroup::addValveToGroup(Valve_Abstract& valve) {
    valves_.appendIfNotInList(&valve);
}


void ValveGroup::removeValveFromGroup(Valve_Abstract& valve) {
    valves_.removeAllEqual(&valve);
}


void ValveGroup::setValves(bool open) {

    valvesState_ = open;

    for (uint32_t i = 0; i < valves_.getNumItems(); i++) valves_[i]->open(valvesState_);

}

bool ValveGroup::isOpen() {
    if (valves_.getNumItems() == 0) return false;
    return valvesState_;
}
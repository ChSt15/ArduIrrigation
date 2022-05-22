#ifndef VALVE_GROUP_H
#define VALVE_GROUP_H


#include "../IOControl/Valves/valve_abstract.h"



class ValveGroup {
private:

    List<Valve_Abstract*> valves_;

    bool valvesState_ = false;


public:

    ValveGroup();

    ValveGroup(Valve_Abstract& valve);


    void addValveToGroup(Valve_Abstract& valve);

    void removeValveFromGroup(Valve_Abstract& valve);

    void setValves(bool open);

    bool isOpen();


};


#endif
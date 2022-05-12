#include "electromagnet_valve.h"



Valve_Electromagnet::Valve_Electromagnet(GPIO_HAL_Abstract& outA, GPIO_HAL_Abstract& outB): Task_Threading("Valve Control", eTaskPriority_t::eTaskPriority_Realtime, 1*SECONDS), outA_(outA), outB_(outB) {
    
}


void Valve_Electromagnet::openValve(bool open) {

    outA_.setPinValue(open);
    outB_.setPinValue(false);

    isOpen_ = open;

}

bool Valve_Electromagnet::isOpen() {
    return isOpen_;
}


void Valve_Electromagnet::init() {

    outA_.init();
    outB_.init();

    outA_.setPinMode(eGPIO_IOMODE_t::eGPIO_IOMODE_OUTPUT);
    outB_.setPinMode(eGPIO_IOMODE_t::eGPIO_IOMODE_OUTPUT);

    openValve(false);

}

void Valve_Electromagnet::thread() {

    suspendUntil(END_OF_TIME);

}

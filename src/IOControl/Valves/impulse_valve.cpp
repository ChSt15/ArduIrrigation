#include "impulse_valve.h"

#include "Arduino.h"


Valve_Impulse* Valve_Impulse::valveChange_ = nullptr;


Valve_Impulse::Valve_Impulse(GPIO_HAL_Abstract& outA, GPIO_HAL_Abstract& outB, int64_t pulseTime_ns): Task_Threading("Valve Control", eTaskPriority_t::eTaskPriority_Realtime, SECONDS/10), outA_(outA), outB_(outB) {
    pulseTime_ = pulseTime_ns;
}


void Valve_Impulse::openValve(bool open) {

    if (open == stateCommand_) return;

    stateCommand_ = open;

    //outA_.setPinValue(open);
    //outB_.setPinValue(!open);

    //isOpen_ = open;

    //this->suspendUntil(NOW() + pulseTime_);

    this->suspendUntil(0);

}

bool Valve_Impulse::isOpen() {
    return isOpen_;
}


void Valve_Impulse::init() {

    outA_.init();
    outB_.init();

    outA_.setPinMode(eGPIO_IOMODE_t::eGPIO_IOMODE_OUTPUT);
    outB_.setPinMode(eGPIO_IOMODE_t::eGPIO_IOMODE_OUTPUT);

    //openValve(false);

}

void Valve_Impulse::thread() {

    if (isOpen_ != stateCommand_ || firstRun_) {

        if (valveChange_ != this && valveChange_ != nullptr) {
            this->suspendUntil(NOW() + valveChange_->pulseTime_ + 10*MILLISECONDS);
        }
        else {

            outA_.setPinValue(stateCommand_);
            outB_.setPinValue(!stateCommand_);

            isOpen_ = stateCommand_;

            this->suspendUntil(NOW() + pulseTime_);

            valveChange_ = this;

            firstRun_ = false;

        }

    } else {

        outA_.setPinValue(false);
        outB_.setPinValue(false);

        suspendUntil(END_OF_TIME);

        valveChange_ = nullptr;

    }

    

}

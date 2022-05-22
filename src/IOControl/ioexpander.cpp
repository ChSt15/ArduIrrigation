#include "ArduIrrigationBase/IOControl/IOExpander/ioexpander.h"


//Section for IOExpander_MCP23S17 class implementation


IOExpander_MCP23S17::IOExpander_MCP23S17(SPIClass& bus, uint32_t cs, uint8_t addr): Task_Threading("IOExpanderMCP23S17", eTaskPriority_t::eTaskPriority_Realtime, 1*SECONDS), expander_(bus, cs, addr) {

    for (uint8_t i = 0; i < 16; i++) {
        gpioPins_[i] = GPIO_MCP23S17(*this, i);
    }

}


void IOExpander_MCP23S17::init() {

    expander_.begin();

    for (uint8_t i = 0; i < 16; i++) {
        expander_.pinMode(i, INPUT);
        pinModes_[i] = eGPIO_IOMODE_t::eGPIO_IOMODE_INPUT;
    }

}


void IOExpander_MCP23S17::setPinMode(uint32_t pin, eGPIO_IOMODE_t mode) {

    if (pin >= 16 || mode == pinModes_[pin]) return;

    if (mode == eGPIO_IOMODE_t::eGPIO_IOMODE_OUTPUT) expander_.pinMode(pin, OUTPUT);
    else expander_.pinMode(pin, INPUT);

    pinModes_[pin] = mode;

}


eGPIO_IOMODE_t IOExpander_MCP23S17::getPinMode(uint32_t pin) {

    if (pin >= 16) return eGPIO_IOMODE_t::eGPIO_IOMODE_NONE;

    return pinModes_[pin];

}

void IOExpander_MCP23S17::setPinValue(uint32_t pin, bool value) {

    if (pin >= 16 || pinModes_[pin] != eGPIO_IOMODE_t::eGPIO_IOMODE_OUTPUT || value == pinStates_[pin]) return;

    expander_.digitalWrite(pin, value);

    pinStates_[pin] = value;

}


bool IOExpander_MCP23S17::getPinValue(uint32_t pin) {

    if (pin >= 16) return false;

    if (pinModes_[pin] == eGPIO_IOMODE_t::eGPIO_IOMODE_INPUT) pinStates_[pin] = expander_.digitalRead(pin);

    return pinStates_[pin];

}


GPIO_MCP23S17& IOExpander_MCP23S17::getGPIOPinRef(uint8_t pin) {
    return gpioPins_[min(pin, uint8_t(15))];
}



//Section for GPIO_MCP23S17 class implementation


GPIO_MCP23S17::GPIO_MCP23S17() {
    pin_ = 0;
    expander_ = nullptr;
}


GPIO_MCP23S17::GPIO_MCP23S17(IOExpander_MCP23S17& expander, uint8_t pin) {
    pin_ = pin;
    expander_ = &expander;
}


void GPIO_MCP23S17::init() {};


uint32_t GPIO_MCP23S17::getPin() { return pin_; };


void GPIO_MCP23S17::setPinValue(bool value) {
    expander_->setPinValue(pin_, value);
};


bool GPIO_MCP23S17::getPinValue() {
    return expander_->getPinValue(pin_);
};


void GPIO_MCP23S17::setPinMode(eGPIO_IOMODE_t mode) {
    expander_->setPinMode(pin_, mode);
};


eGPIO_IOMODE_t GPIO_MCP23S17::getPinMode() {
    return expander_->getPinMode(pin_);
};


void GPIO_MCP23S17::setPinPull(eGPIO_PULLMODE_t pull) {};


eGPIO_PULLMODE_t GPIO_MCP23S17::getPinPull() {
    return eGPIO_PULLMODE_t::eGPIO_PULLMODE_NONE;
};
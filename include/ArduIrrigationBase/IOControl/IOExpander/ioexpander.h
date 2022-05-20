#ifndef IO_EXPANDER_H
#define IO_EXPANDER_H


#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"

#include "KraftKontrol/hal/gpio_hal_abstract.h"

#include "lib/MCP23S17/MCP23S17.h"

#include "SPI.h"

#include "stdint.h"


/**
 * @brief Class to control MCP23S17 IC.
 * @see GPIO_MCP23S17 for general GPIO abstraction of MCP23S17
 * Forward declaration of IOExpander_MCP23S17
 * 
 */
class IOExpander_MCP23S17;


/**
 * @brief Single GPIO pin of MCP23S17.
 * 
 */
class GPIO_MCP23S17: public GPIO_HAL_Abstract {
private:

    IOExpander_MCP23S17* expander_;
    uint8_t pin_ = 0;


public:

    GPIO_MCP23S17();

    GPIO_MCP23S17(IOExpander_MCP23S17& expander, uint8_t pin);

    /**
     * Initialises GPIO.
     */
    void init() override;

    /**
     * @returns Which pin this controls.
     */
    uint32_t getPin() override;

    /**
     * Sets pin value.
     * @param value Which value to set pin to.
     */
    void setPinValue(bool value) override;

    /**
     * @returns current pin value.
     */
    bool getPinValue() override;

    /**
     * Sets pin to given mode. Input/output.
     * @param mode Which mode to set pin to.
     */
    void setPinMode(eGPIO_IOMODE_t mode) override;

    /**
     * @returns current pin mode.
     */
    eGPIO_IOMODE_t getPinMode() override;

    /**
     * *Currently not supported*
     * 
     * Sets pin to use given pullup or pulldown resistor or to remove.
     * @param mode Which pull mode to set pin to.
     */
    void setPinPull(eGPIO_PULLMODE_t pull) override;

    /**
     * *Currently not supported*
     * 
     * @returns current pin pullmode.
     */
    eGPIO_PULLMODE_t getPinPull() override;


};


/**
 * @brief Class to control MCP23S17 IC.
 * @see GPIO_MCP23S17 for general GPIO abstraction of MCP23S17
 * 
 */
class IOExpander_MCP23S17: public Task_Threading {
private:

    MCP23S17 expander_;

    GPIO_MCP23S17 gpioPins_[16];

    eGPIO_IOMODE_t pinModes_[16];
    bool pinStates_[16];

    void init() override;

    
public:

    IOExpander_MCP23S17(SPIClass& bus, uint32_t cs, uint8_t addr);

    void setPinMode(uint32_t pin, eGPIO_IOMODE_t mode);
    eGPIO_IOMODE_t getPinMode(uint32_t pin);

    void setPinValue(uint32_t pin, bool value);
    bool getPinValue(uint32_t pin);

    GPIO_MCP23S17& getGPIOPinRef(uint8_t pin);
    

};



#endif
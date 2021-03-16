#include "IOControl.h"



namespace IOControl {



    namespace {

        struct ValveCommand {
            uint8_t valve = -1;
            bool state = false;
            uint32_t time = 0;
        };

        enum OUTPUT_STATE {
            OPEN,
            POSITIVE,
            NEGATIVE
        };


        const byte valveAPins[8] = {Valve1APin, Valve2APin, Valve3APin, Valve4APin, Valve5APin, Valve6APin, Valve7APin, Valve8APin};
        const byte valveBPins[8] = {Valve1BPin, Valve2BPin, Valve3BPin, Valve4BPin, Valve5BPin, Valve6BPin, Valve7BPin, Valve8BPin};

        const byte gpioExpanderAddr = 0;
        const int gpioExpanderCSPin = 13;

        const int vExtPin = VEXTCONTROLPIN;

        MCP23S17 _expander(SPI, gpioExpanderCSPin, gpioExpanderAddr);

        //Queue <ValveCommand> _valveCommands(VALVECOMMANDS_QUEUE_SIZE);
        //Queue <ValveCommand> _valveStatesChange(VALVECOMMANDS_QUEUE_SIZE);
    
        float _vBatBatteryFactor = 1.0f;

        //OUTPUT_STATE _outputStates[8];

        //bool _valveStates[8];
        bool _vExtActive = false;

        int8_t _setStates[8];
        int8_t _isStates[8];

        uint32_t _lastOutputUpdate = 0;

    }



    uint16_t _getADC(int pin, bool extPower) {

        if (extPower && !_vExtActive) {
            digitalWrite(vExtPin, HIGH); //power up external power
            delay(10); //Wait for power to stabilise
        }

        uint32_t measurement = 0;

        for (int i = 0; i < ADC_NUM_MEASUREMENTS; i++) {
            measurement += analogRead(pin);
        }

        measurement /= ADC_NUM_MEASUREMENTS;

        if (extPower && !_vExtActive) digitalWrite(vExtPin, LOW);

        return measurement;

    }



    void setVBatFactor(float factor) {_vBatBatteryFactor = factor;}
    bool getValveState(uint8_t valve) {return _isStates[constrain(valve, 0, 7)] == 1 ? IOVALVE_OPEN:IOVALVE_CLOSE;}
    float getVBatFactor() {return _vBatBatteryFactor;}



    void loop() {


        if (millis() - _lastOutputUpdate >= VALVE_CHANGE_TIME) {
            _lastOutputUpdate = millis();

            for (int i = 0; i < 8; i++) {
                _expander.digitalWrite(valveAPins[i], LOW);
                _expander.digitalWrite(valveBPins[i], LOW);
            }

            _vExtActive = false;

            for (int i = 0; i < 8; i++) {

                if (_setStates[i] != _isStates[i]) { // if state is different then update and then leave 
                    _isStates[i] = _setStates[i];

                    _vExtActive = true;

                    if (_setStates[i] == 1) {
                        _expander.digitalWrite(valveAPins[i], HIGH);
                        _expander.digitalWrite(valveBPins[i], LOW);
                    } else {
                        _expander.digitalWrite(valveAPins[i], LOW);
                        _expander.digitalWrite(valveBPins[i], HIGH);
                    }

                    break;
                }

            }

            digitalWrite(vExtPin, true);

        }

    }



    void begin() {

        _expander.begin();
        for (int i = 0; i < 16; i++) _expander.pinMode(i, OUTPUT); // Sets all pins to outputs
        _expander.writePort(0x0000); //sets all pins to low

        for (int i = 0; i < 8; i++) _setStates[i] = _isStates[i] = 0;

        pinMode(ADCInput1Pin, INPUT);
        pinMode(ADCInput2Pin, INPUT);
        pinMode(ADCINPUTVBATPIN, INPUT);

        pinMode(vExtPin, OUTPUT);
        digitalWrite(vExtPin, LOW);

    }



    float getMoisture(uint8_t sensor) {

        if (sensor == 0) {
            return (float)_getADC(ADCInput1Pin, true)*ADCInput1Factor;
        } else if (sensor == 1) {
            return (float)_getADC(ADCInput2Pin, true)*ADCInput2Factor;
        }

        return 0;

    }



    float getBatteryVoltage() {
        return (float)_getADC(ADCINPUTVBATPIN, false)*ADCINPUTVBATFACTOR*_vBatBatteryFactor;
    }



    bool setValveState(byte valve, bool state) {

        /*if (_valveCommands.isFull()) return false;

        ValveCommand command;

        command.valve = valve;
        command.state = state;

        _valveCommands.enQueue(command);*/

        valve = min(valve, byte(7));

        if (state) _setStates[valve] = 1;
        else _setStates[valve] = -1;

        return true;

    }


}

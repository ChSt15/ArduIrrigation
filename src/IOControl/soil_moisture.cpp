#include "ArduIrrigationBase/IOControl/SoilMoisture/soil_moisture.h"



SoilMoisture::SoilMoisture(uint32_t pin, float updateRate): Task_Threading("Soil Moisture Sensor", eTaskPriority_t::eTaskPriority_High, SECONDS/updateRate) {
    pin_ = pin;
}

void SoilMoisture::init() {

    pinMode(pin_, INPUT);

}

void SoilMoisture::thread() {

    moisturePercent_ = moisturePercent_*0.5 + (float)analogReadMilliVolts(pin_)/3300.0f*0.5;

}

float SoilMoisture::getMoisturePercent() {
    return moisturePercent_;
}


#include "sensors.h"



namespace Sensors {



    namespace {

        Adafruit_BME280 bme;

        uint32_t lastMeasurement = 0;

        bool enabledGroundSensors[2];

    }



    void loop() {

        if (millis() - lastMeasurement >= AIRDATA_MEASUREMENT_INTERVAL) {
            lastMeasurement = millis();

            bme.takeForcedMeasurement();

            float mois = bme.readHumidity();
            float pres = bme.readPressure();
            float temp = bme.readTemperature();

            systemStatusGlobal.airPressure = pres;
            systemStatusGlobal.airHumidity = mois;
            systemStatusGlobal.airTemperature = temp;

            systemStatusGlobal.soilMoisture[0] = (float)IOControl::_getADC(ADCInput1Pin, true)*ADCInput1Factor;
            systemStatusGlobal.soilMoisture[1] = (float)IOControl::_getADC(ADCInput2Pin, true)*ADCInput2Factor;

        }

    }


    void begin() {

        if (bme.begin(0x76, &Wire)) {

            systemStatusGlobal.airSensorStatus = DEVICESTATUS::RUNNING;

            bme.setSampling(
                Adafruit_BME280::MODE_FORCED,
                Adafruit_BME280::SAMPLING_X1, // temperature
                Adafruit_BME280::SAMPLING_X1, // pressure
                Adafruit_BME280::SAMPLING_X1, // humidity
                Adafruit_BME280::FILTER_OFF   
            );

        } else systemStatusGlobal.airSensorStatus = DEVICESTATUS::FAILURE;

        for (int i = 0; i < sizeof(enabledGroundSensors[0])/sizeof(enabledGroundSensors); i++) {
            enabledGroundSensors[i] = false;
        }

    }



    void enableGroundMoistureSensor(uint8_t sensorNum, bool enable) {

        sensorNum = min(sensorNum, uint8_t(0));

        enabledGroundSensors[sensorNum] = enable;

    }


}

#include <Arduino.h>
#include <Wire.h>

#include "EEPROM.h"
#include "KraftKontrol/modules/datalink_modules/sx1280_lora_driver.h"

#include "IOControl/IOExpander/ioexpander.h"

#include "IOControl/Valves/impulse_valve.h"

#include "IOControl/SoilMoisture/soil_moisture.h"

#include "Timing/rtc_ds3231.h"

#include "Timing/watering_timedate.h"
#include "Timing/watering_cycle.h"

#include "comms/Physical/physical_link_topic.h"
#include "comms/Datalink/datalink.h"

#include "definitions.h"

#include "SSD1306Wire.h"


SX1280Driver loraDriver(RFBUSY, TXEN, RXEN, DIO1, NRESET, NSS, 10);

IOExpander_MCP23S17 ioExpander(SPI, GPIOExpanderCSPin, GPIOExpanderAddr);

Valve_Impulse valve1(ioExpander.getGPIOPinRef(Valve1BPin), ioExpander.getGPIOPinRef(Valve1APin));
Valve_Impulse valve2(ioExpander.getGPIOPinRef(Valve2BPin), ioExpander.getGPIOPinRef(Valve2APin));
Valve_Impulse valve3(ioExpander.getGPIOPinRef(Valve3BPin), ioExpander.getGPIOPinRef(Valve3APin));


//WateringEvent event1(valve, 23, 27, 10*SECONDS);

//Watering_Cycle cycle1(valve1, 2*HOURS, 30*SECONDS, 4*HOURS, 19*HOURS+5*MINUTES);

Watering_Cycle cycle11(valve1, 1*HOURS, 3*MINUTES, 12*HOURS, 23*HOURS);
Watering_Cycle cycle12(valve1, 1*HOURS, 3*MINUTES, 1*HOURS, 8*HOURS);

Watering_Cycle cycle21(valve2, 1*HOURS, 2*MINUTES, 12*HOURS + 4*MINUTES, 23*HOURS);
Watering_Cycle cycle22(valve2, 1*HOURS, 2*MINUTES, 1*HOURS + 4*MINUTES, 8*HOURS);

Watering_Cycle cycle31(valve3, 1*HOURS, 3*MINUTES, 12*HOURS + 8*MINUTES, 23*HOURS);
Watering_Cycle cycle32(valve3, 1*HOURS, 3*MINUTES, 1*HOURS + 8*MINUTES, 8*HOURS);
//Watering_Cycle cycle11(valve1, 1*HOURS, 3*MINUTES, 12*HOURS, 8*HOURS);
//Watering_Cycle cycle2(valve2, 1*HOURS, 3*MINUTES, 12*HOURS+2*MINUTES, 8*HOURS);
//Watering_Cycle cycle3(valve3, 1*HOURS, 3*MINUTES, 12*HOURS+4*MINUTES, 8*HOURS);


//Watering_Cycle cycle1(valve, 1*MINUTES, 10*SECONDS, 0*HOURS, 23*HOURS);


//SoilMoisture soilMoisture(ADCInput1Pin, 1);

Clock_DS3231 rtcClock;



class BatteryVoltage: public Task_Threading {
private:

    float voltage = 0;

    bool powerSaveMode = false;

public:

    BatteryVoltage(): Task_Threading("VBat Monitor", eTaskPriority_t::eTaskPriority_VeryHigh, 1*SECONDS) {}

    void init() {

        voltage = getVoltage();

    }


    void thread() {

        voltage = voltage*0.9 + getVoltage()*0.1;

        if (voltage < 3.4) powerSaveMode = true;

        if (powerSaveMode && !Valve_Abstract::getlockAllValveState()) {
            Valve_Abstract::forceAllValvesShut(true);
            Valve_Abstract::lockAllValveState(true);
        } 
        
        if (voltage > 3.5 && powerSaveMode) {
            Valve_Abstract::lockAllValveState(false);
            powerSaveMode = false;
        }

    }

    bool getPowersaveEnable() {
        return powerSaveMode;
    }

    float getBatteryVoltage() {
        return voltage;
    }

    float getBatteryPercent() {
        float batPercent = (voltage-3.3)/(4.2-3.3)*100;
        return constrain(batPercent, 0, 100);
    }


private:

    float getVoltage() {

        return (float)analogReadMilliVolts(ADCINPUTVBATPIN)*2/1000-0.08;

    }



} vBatMonitor;


class Display: public Task_Threading {
private:

    SSD1306Wire display_ = SSD1306Wire(0x3c, SDA, SCL);
    bool displayOn = false;

    bool on = true;

    float avg1 = 0;
    float avg2 = 0;
    float avg3 = 0;

    float thres = 10;

    int64_t valveOpenStart = 0;
    bool valveState = 0;

    Buffer<float, 3> touchBuffer1;
    Buffer<float, 3> touchBuffer2;
    Buffer<float, 3> touchBuffer3;

public:

    Display(): Task_Threading("Display Control", eTaskPriority_t::eTaskPriority_High, 500*MILLISECONDS) {}


    void init() {

        Wire.begin();

        display_.init();

        display_.setContrast(255);
        display_.setBrightness(255);
        display_.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        display_.setFont(ArialMT_Plain_10);

        pinMode(TOUCHINPUTPIN2, INPUT);
        pinMode(ADCINPUTVBATPIN, INPUT);

        touchRead(TOUCHINPUTPIN1);
        touchRead(TOUCHINPUTPIN2);
        touchRead(TOUCHINPUTPIN3);

        avg1 = avg2 = avg3 = 0;
        for (int i = 0; i < 50; i++) {
            avg1 += touchRead(TOUCHINPUTPIN1);
            avg2 += touchRead(TOUCHINPUTPIN2);
            avg3 += touchRead(TOUCHINPUTPIN3);
            touchBuffer1.placeBack(touchRead(TOUCHINPUTPIN1), true);
            touchBuffer2.placeBack(touchRead(TOUCHINPUTPIN2), true);
            touchBuffer3.placeBack(touchRead(TOUCHINPUTPIN3), true);
        }
        avg1 /= 50;
        avg2 /= 50;
        avg3 /= 50;

    }


    void thread() {

        

        touchBuffer1.placeBack(touchRead(TOUCHINPUTPIN1), true);
        touchBuffer2.placeBack(touchRead(TOUCHINPUTPIN2), true);
        touchBuffer3.placeBack(touchRead(TOUCHINPUTPIN3), true);

        float touch1 = touchBuffer1.getMedian();
        float touch2 = touchBuffer2.getMedian();
        float touch3 = touchBuffer3.getMedian();

        //Serial.println(String() + "Touch: " + touch + ", avg: " + avg);

        if (valveState && NOW() - valveOpenStart > 10*MINUTES) {
            valveState = false;
            if (valve1.isOpen()) valve1.open(false);
            if (valve2.isOpen()) valve2.open(false);
            if (valve3.isOpen()) valve3.open(false);
        }

        if (touch1 < avg1-thres || NOW() < 5*SECONDS) {

            if (touch2 < avg2-thres) {

                avg2 = avg2*0.995 + float(touch2)*0.005;

                if (!valve1.isOpen()) valve1.open(true);
                if (!valve2.isOpen()) valve2.open(true);
                if (!valve3.isOpen()) valve3.open(true);

                valveOpenStart = NOW();
                valveState = true;

            }

            if (touch3 < avg3-thres) {

                avg3 = avg3*0.995 + float(touch3)*0.005;

                if (valve1.isOpen()) valve1.open(false);
                if (valve2.isOpen()) valve2.open(false);
                if (valve3.isOpen()) valve3.open(false);

                valveOpenStart = 0;
                valveState = false;

            }

            avg1 = avg1*0.995 + float(touch1)*0.005;

            float bat = vBatMonitor.getBatteryVoltage();
            float batPercent = vBatMonitor.getBatteryPercent();
            
            TimeDate time = SystemTime::getCurrentDateTime();
            
            TimeDate next = time + TimeSpan(10*YEARS);

            for (int i = 0; i < Watering_Abstract::getWateringTimeList().getNumItems(); i++) {
                TimeDate buf = Watering_Abstract::getWateringTimeList()[i]->getNextWateringTime();
                if (next.unixtime() > buf.unixtime()) next = buf;
            }
            
            if (!displayOn) {
                displayOn = true;
                display_.displayOn();
            }
            
            
            display_.clear();

            display_.drawString(64, 10, String("Akku: ") + int(batPercent) + "%" + "(" + bat + "V)");
            display_.drawString(64, 20, String("Strom Sparen: ") + (vBatMonitor.getPowersaveEnable()?"Ja":"Nein"));
            display_.drawString(64, 30, String("Zeit: ") + time.hour() + ":" + time.minute() + ":" + time.second());
            display_.drawString(64, 40, String("Nächste: ") + next.hour() + ":" + next.minute() + ":" + next.second() + " in " + String((double)(next - time).nanosecondsTime()/SECONDS, 0) + "s");
            display_.drawString(64, 50, String("Ventile: 1:") + (valve1.isOpen()?"Auf":"Zu") + " 2:" + (valve2.isOpen()?"Auf":"Zu") + " 3:" + (valve3.isOpen()?"Auf":"Zu"));


            display_.display();

        } else {

            avg1 = avg1*0.9 + touch1*0.1;
            avg2 = avg2*0.9 + touch2*0.1;
            avg3 = avg3*0.9 + touch3*0.1;

            
            if (displayOn) {
                displayOn = false;
                display_.displayOff();
            }

        }

    }



} display;


class TestingClass: public Task_Threading {
public:

    TestingClass(): Task_Threading("Testing Thread", eTaskPriority_t::eTaskPriority_Realtime, 1*SECONDS) {}

    void thread() override {

        //printThreads();

        if (NOW() > 5*MINUTES) esp_restart();

    }


    void printThreads() {

        Serial.println(String("Num tasks: ") + Task_Threading::getTaskList().getNumItems());
        Serial.println(String("Sleep Time: ") + Task_Threading::getSleepPercent()*100);
        Serial.println("Task usages: ");
        Serial.println(String("CPU Usage: ") + String(Task_Threading::getSchedulerSystemUsage()*100.0, 2));
        for (uint32_t i = 0; i < Task_Threading::getTaskList().getNumItems(); i++) {
            Serial.println(String() + "    - Usage: " + String(Task_Threading::getTaskList()[i]->getTaskSystemUsage()*100, 2) + "\t rate: " + Task_Threading::getTaskList()[i]->getRunRate() + "\t, Name: " + Task_Threading::getTaskList()[i]->getTaskName());
        }


    }


} testingClass;



/*Topic<DataFrame> topicLinkTest;

PhysicalLink_Topic physicalLinkTest1(topicLinkTest);
DataLink dataLink1(physicalLinkTest1);

PhysicalLink_Topic physicalLinkTest2(topicLinkTest);
DataLink dataLink2(physicalLinkTest2);


class LinkTestSend: public Task_Threading {
public:

    LinkTestSend(): Task_Threading("Link Test Send", eTaskPriority_t::eTaskPriority_Realtime, 1*SECONDS) {}

    void thread() override {

        uint8_t array[5];

        array[0] = 1;
        array[1] = 2;
        array[2] = 3;
        array[3] = 6;
        array[4] = 60;
        DataFrame frame(array, 5);

        dataLink1.getSendDataTopic().publish(frame);

        array[0] = 1;
        array[1] = 8;
        array[2] = 4;
        array[3] = 6;
        array[4] = 60;
        frame = DataFrame(array, 5);
        dataLink1.getSendDataTopic().publish(frame);

        array[0] = 1;
        array[1] = 9;
        array[2] = 1;
        array[3] = 6;
        array[4] = 60;
        frame = DataFrame(array, 5);
        dataLink1.getSendDataTopic().publish(frame);

        Serial.println("Sending time!");

    }


} linkTestSend;

class LinkTestReceive: public Task_Threading {
public:

    LinkTestReceive(): Task_Threading("Link Test Receive", eTaskPriority_t::eTaskPriority_Realtime, 100*MILLISECONDS) {}

    Buffer_Subscriber<DataFrame, 10> subr_;

    void init() override {

        subr_.subscribe(dataLink2.getReceivedDataTopic());

    }

    void thread() override {

        while (subr_.available()) {

            DataFrame frame;
            subr_.takeBack(frame);

            uint8_t array[5];
            frame.copyContentsToArray(array, 5);

            Serial.println(String() + "Received size: " + frame.getDataLength() + ". With Data: " + String(array[0]) + "," + String(array[1]) + "," + String(array[2]) + "," + String(array[3]) + "," + String(array[4]) + ". at time " + (double)NOW()/SECONDS);

        }

    }


} linkTestReceive;*/


void sleepForTime(int64_t sleepTime_ns) {

    esp_sleep_enable_timer_wakeup(sleepTime_ns/MICROSECONDS);

    esp_light_sleep_start();

}


void setup() {

    setCpuFrequencyMhz(CPU_SPEED);

    pinMode(VEXTCONTROLPIN, OUTPUT);
    digitalWrite(VEXTCONTROLPIN, HIGH);

    Serial.begin(115200);

    Wire.begin();

    SystemTime::setClockSource(rtcClock);
    //rtcClock.setTime(TimeDate(2022, 4, 20, 20, 58, 00).unixtime()*SECONDS);
    //rtcClock.setTime(TimeDate(__DATE__, __TIME__).unixtime()*SECONDS);
    rtcClock.init();

    //SystemTime::setTime(TimeDate(2022, 4, 20, 2, 7, 40));

    Task_Threading::setSleepFunction(sleepForTime);

    Task_Threading::schedulerInitTasks();

    loraDriver.setLoRaParams(2445000000UL, LORA_SF12, LORA_BW_0200, LORA_CR_4_8, false);
    loraDriver.stopReceiving();
    //loraDriver.startReceiving();

}



void loop() {

    Task_Threading::schedulerTick();

}






/*void sleepForTime(uint32_t sleepLength) {

    esp_sleep_enable_timer_wakeup((uint64_t)sleepLength*1000);

    //Serial.println("Sleep Start!");

    delay(10);

    esp_light_sleep_start();

    //Serial.println("Wake Up!");

    

}



void sleepWithInt(uint32_t sleeplength) {

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 1);
    sleepForTime(sleeplength);

}*/

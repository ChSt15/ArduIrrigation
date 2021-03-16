#include "system/system.hpp"


SystemStatus systemStatusGlobal;
ValveTiming valveTiming[8];
RTC_DS3231 rtc;

uint8_t latestNodeStatus = 0;
int8_t nodeStatusRSSI[MAX_NUM_NODES];
int8_t nodeStatusSNR[MAX_NUM_NODES];
SystemStatus nodeStatus[MAX_NUM_NODES];
uint32_t nodeStatusCounter[MAX_NUM_NODES];

uint8_t latestNodeTime = 0;
int8_t nodeTimeRSSI[MAX_NUM_NODES];
int8_t nodeTimeSNR[MAX_NUM_NODES];
DateTime nodeTime[MAX_NUM_NODES];
uint32_t nodeTimeCounter[MAX_NUM_NODES];


void progInit(void) {

    //setCpuFrequencyMhz(CPU_SPEED); // Slow CPU down to save energy. We dont gotta go fast!


    //Initialise core funtions
    Wire.begin(-1, -1, 100000); // default pins and 100kHz speed

    SPI.begin();
    SPI.setFrequency(10000000L);

    Serial.begin(115200);


    //Initialise EEPROM and check if running
    initEEPROM();

    SystemStatus statusBuffer;

    if (loadSystemStatus(&statusBuffer)) {

        systemStatusGlobal = statusBuffer;

        systemStatusGlobal.eepromStatus = DEVICESTATUS::RUNNING;
        Serial.println("EEPROM Success. Node ID: " + String(systemStatusGlobal.nodeID));

        systemStatusGlobal.bootCount = getEEPROMBootCount() + 1;
        setEEPROMBootCount(systemStatusGlobal.bootCount);

    } else {

        systemStatusGlobal.eepromStatus = DEVICESTATUS::FAILURE;
        Serial.println("EEPROM FAIL!");

        /*
            if EEPROM fails then we do not know our battery voltage and node ID.
            In this case we must assume the battery is always critical and cannot connect to radio network
        */

    }

    //userSettings();

    //Initialise IOControl::and if EEPROM running then update vbat factor
    IOControl::begin();

    //float vBat = IOControl::getBatteryVoltage();

    if (systemStatusGlobal.eepromStatus == DEVICESTATUS::RUNNING) IOControl::setVBatFactor(systemStatusGlobal.vBatCorrectionFactor);

    systemStatusGlobal.vBat = IOControl::getBatteryVoltage();

    IOControl::setValveState(0, IOVALVE_CLOSE);
    IOControl::setValveState(1, IOVALVE_CLOSE);
    IOControl::setValveState(2, IOVALVE_CLOSE);
    IOControl::setValveState(3, IOVALVE_CLOSE);
    IOControl::setValveState(4, IOVALVE_CLOSE);
    IOControl::setValveState(5, IOVALVE_CLOSE);
    IOControl::setValveState(6, IOVALVE_CLOSE);
    IOControl::setValveState(7, IOVALVE_CLOSE);



    //Initialise Sensors
    Sensors::begin();


    //Initialise Real Time Clock
    if (rtc.begin()) {

        systemStatusGlobal.rtcStatus = DEVICESTATUS::RUNNING;

        systemStatusGlobal.systemStartTime = systemStatusGlobal.now = rtc.now();

    }


    //Initialise wifi if enabled
    if (systemStatusGlobal.eepromStatus == DEVICESTATUS::RUNNING && systemStatusGlobal.wifiEnabled) {

        //wifi start and connect here

    } else {

        //wifi disable here

    }


    //Initialise LoRa radios and setup Network
    Network::setup();
    Network::wake();



    //Initialise UserInterface
    if (UserInterface::begin()) {
        systemStatusGlobal.interfaceStatus = DEVICESTATUS::RUNNING;
    } else systemStatusGlobal.interfaceStatus = DEVICESTATUS::FAILURE;
    


    

    //Give User and programmer useful data after start is done and wait 
    printSystemData();

}



void progLoop(void) {

    static uint32_t lastSystemStatusUpdate = 0;
    static uint32_t lastTimeUpdate = 0;
    static uint32_t lastTimeAddition = 0;


    Sleep::loopSleep(); //Check if energy can be saved by sleeping

    //Correct time by reading from RTC
    if (millis() - lastTimeUpdate >= RTC_UPDATE_INTERVAL) {
        lastTimeUpdate = millis();

        systemStatusGlobal.now = rtc.now();
        lastTimeAddition = millis();

    }
    //Update time exacly when 1s passes
    uint32_t dTime = (millis() - lastTimeAddition)/1000;
    if (dTime != 0) {
        systemStatusGlobal.now = systemStatusGlobal.now + TimeSpan(dTime);
        lastTimeAddition += 1000;
    }


    IOControl::loop();
    systemStatusGlobal.vBat = systemStatusGlobal.vBat*0.95f + IOControl::getBatteryVoltage()*0.05f;    

    if (systemStatusGlobal.vBat > POWERSHUTDOWN_THRESHOLD) {

        if (systemStatusGlobal.vBat < POWERSAVING_ENABLE_THRESHOLD && systemStatusGlobal.systemPower != POWERSTATUS::POWERSAVING) {
            systemStatusGlobal.systemPower = POWERSTATUS::POWERSAVING;
            Network::sleep();
        } else if (systemStatusGlobal.vBat > POWERSAVING_DISABLE_THRESHOLD && systemStatusGlobal.systemPower != POWERSTATUS::POWERNORMAL) {
            systemStatusGlobal.systemPower = POWERSTATUS::POWERNORMAL;
            Network::wake();
        }

    } else if (systemStatusGlobal.vBat < POWERSHUTDOWN_THRESHOLD && systemStatusGlobal.systemPower != POWERSTATUS::POWERSHUTDOWN) {
        systemStatusGlobal.systemPower = POWERSTATUS::POWERSHUTDOWN;
        Network::shutdown();
    }


    if (millis() - lastSystemStatusUpdate >= 5000) {
        lastSystemStatusUpdate = millis() - random(1000);

        Network::systemDataSend(systemStatusGlobal, NODE_BROADCAST);

    }

    Network::loop();


    Sensors::loop();


    UserInterface::loop(systemStatusGlobal);

}



void printSystemData() {

    /*Serial.println();
    Serial.println("Program finished initialising.");
    Serial.println("Node ID: " + String(systemStatusGlobal.nodeID));
    Serial.println("Software version: " + String(systemStatusGlobal.softwareVersion));
    Serial.println(systemStatusGlobal.wifiEnabled ? "Wifi is enabled":"Wifi is disabled");
    Serial.print("The date is:" + String(systemStatusGlobal.now.day()) + "." + String(systemStatusGlobal.now.month()) + "." + String(systemStatusGlobal.now.year()));
    Serial.println(", and time is: " + String(systemStatusGlobal.now.hour()) + ":" + String(systemStatusGlobal.now.minute()) + ":" + String(systemStatusGlobal.now.second()));
    Serial.println("VBat: " + String(IOControl::getBatteryVoltage()));*/

    /*display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);

    display.drawString(0, 0, "Node ID: " + String(systemStatusGlobal.nodeID));
    display.drawString(0, 10, "Datum: " + String(systemStatusGlobal.now.day()) + "." + String(systemStatusGlobal.now.month()) + "." + String(systemStatusGlobal.now.year()));
    display.drawString(0, 20, "Uhrzeit: " + String(systemStatusGlobal.now.hour()) + ":" + String(systemStatusGlobal.now.minute()) + ":" + String(systemStatusGlobal.now.second()));
    display.drawString(0, 30, "VBat: " + String(systemStatusGlobal.vBat));
    display.drawString(0, 40, "Node " + String(latestNodeStatus) + " VBat: " + String(nodeStatus[latestNodeStatus].vBat) + " SNR: " + String(nodeStatusSNR[latestNodeStatus]));
    display.drawString(0, 50, "Zeit: " + String(nodeStatus[latestNodeStatus].now.hour()) + ":" + String(nodeStatus[latestNodeStatus].now.minute()) + ":" + String(nodeStatus[latestNodeStatus].now.second()) + " RSSI: " + String(nodeStatusRSSI[latestNodeStatus]));
    
    display.display();*/

}


void userSettings() {

    systemStatusGlobal.nodeID = 1;
    systemStatusGlobal.vBatCorrectionFactor = 1.10606f;
    
    saveSystemStatus(systemStatusGlobal);
    //setEEPROMBootCount(0);

    //rtc.adjust(DateTime(2020, 10, 29, 16, 7, 0));

}





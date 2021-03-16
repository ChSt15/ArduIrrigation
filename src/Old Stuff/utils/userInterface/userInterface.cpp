#include "userInterface.h"

namespace UserInterface {

    namespace {

        enum BUTTONSTATE {
            NO_PRESS,
            SHORT_PRESS,
            LONG_PRESS,
            HOLD_PRESS
        };

        enum STARTMENU {
            STARTNOTSELECT,
            MAININFOMENU,
            WATERINFOMENU,
            SYSTEMINFOMENU,
            RADIOMENU,
            SETTINGSMENU,
            STARTEND
        } startMenu;

        enum MAININFO {
            MAININFONOTSELECT,
            MAININFO1,
            MAININFO2,
            MAININFOEND
        } mainInfo;

        enum WATERINFO {
            WATERINFONOTSELECT,
            WATERINFO1,
            UPCOMINGWATER,
            WATERMANUAL,
            WATERSETTINGS,
            WATEREND
        } waterInfo;

        enum SYSTEMINFO {
            SYSTEMINFONOTSELECT,
            SYSTEMINFO1,
            SYSTEMEND
        } systemInfo;

        enum RADIOINFO {
            RADIOINFONOTSELECT,
            RADIOINFO1,
            LASTMESSAGES,
            RADIOREQ,
            RADIOEND
        } radioInfo;

        enum SETTINGS {
            SETTINGSNOTSELECT,
            SYSTEMSETTINGS,
            OUTPUTSETTINGS,
            RADIOSETTINGS,
            SETTINGSEND
        } settings;

        enum INTERFACESTATE {
            INTERFACEOFF,
            INTERFACEFRONT,
            INTERFACEACTIVE
        };
        INTERFACESTATE interfaceState = INTERFACESTATE::INTERFACEFRONT;

        
        SSD1306Wire display(0x3c, SDA, SCL);
        bool displayRunning = false;
        uint8_t displayStartRetries = 0;
        uint32_t lastDisplayStartAttempt = 0;

        uint8_t _touchPin1 = TOUCHINPUTPIN1, _touchPin2 = TOUCHINPUTPIN2, _touchPin3 = TOUCHINPUTPIN3;
        float _touchValAVG[3];
        float _touchVal1[3], _touchVal2[3], _touchVal3[3];
        bool touchBool[3] = {false, false, false};
        uint32_t lastPinRead = 0;

        bool interfaceActiveLock = false;

        uint32_t lastInterfaceUpdate = 0;
        uint32_t lastInterfaceInterrupt = 0;

        uint32_t interfaceAutoOffTimeout = INTERFACE_FRONT_TIMEOUT;
        
        uint32_t buttonPressTimestamp[3] = {0,0,0};
        uint32_t buttonPressStage[3] = {0,0,0};

        BUTTONSTATE button[3] = {BUTTONSTATE::NO_PRESS, BUTTONSTATE::NO_PRESS, BUTTONSTATE::NO_PRESS};


        //Front Menu stuff
        uint8_t frontCodeCorrectCount = 0;

        //General Menu stuff
        int32_t menuCursor = 0;
        bool menuCursorSelect = false;
        bool menuCursorLock = false;

        //Data reg stuff
        uint8_t selectedNode = 1;

        //SystemStatus systemStatus;

    }



    bool getTouch(uint8_t pin) {
        return touchBool[constrain(0,2,pin)];
    }
    


    bool begin() {

        pinMode(_touchPin1, INPUT);
        pinMode(_touchPin2, INPUT);
        pinMode(_touchPin3, INPUT);

        uint32_t val1 = 0, val2 = 0, val3 = 0;

        for (int i = 0; i < 20; i++) {

            val1 += touchRead(_touchPin1);
            val2 += touchRead(_touchPin2);
            val3 += touchRead(_touchPin3);

        }

        _touchValAVG[0] = val1/20;
        _touchValAVG[1] = val2/20;
        _touchValAVG[2] = val3/20;

        _touchVal1[0] = _touchVal1[1] = _touchVal1[2] = 0;
        _touchVal2[0] = _touchVal2[1] = _touchVal2[2] = 0;
        _touchVal3[0] = _touchVal3[1] = _touchVal3[2] = 0;

        if (display.init()) {

            display.clear();
            display.setBrightness(255);

            display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
            display.setFont(ArialMT_Plain_10);
            display.drawString(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2, "System Startet...");

            display.display();

            displayRunning = true;

        }

        return true;

    }



    bool interfaceActivity() {return interfaceState != INTERFACESTATE::INTERFACEOFF;}



    uint8_t touchThreshold(uint8_t pin) {
        return _touchValAVG[pin]-PIN_ACTIVE_THRESHOLD;
    }


    void menuNav() {

        //Update Cursor
        if (button[1] == BUTTONSTATE::SHORT_PRESS && !menuCursorLock) {
            menuCursor--;
        } else if (button[2] == BUTTONSTATE::SHORT_PRESS && !menuCursorLock) {
            menuCursor++;
        }


        if (startMenu != STARTMENU::STARTNOTSELECT) {


            switch (startMenu) {

                case STARTMENU::MAININFOMENU :
                    
                    {

                        if (button[0] == BUTTONSTATE::LONG_PRESS) {
                            startMenu = STARTMENU::STARTNOTSELECT;
                            menuCursor = 0;
                        } 

                        menuCursor = constrain(menuCursor, 1, uint8_t(MAININFO::MAININFOEND)-1);     

                        mainInfo = MAININFO(menuCursor);               

                        if (button[2] == BUTTONSTATE::LONG_PRESS && mainInfo == MAININFO::MAININFO2) {
                            setEEPROMBootCount(0);
                            systemStatusGlobal.bootCount = 0;
                        }

                    }

                    break;

                case STARTMENU::RADIOMENU :
                    
                    {

                        if (radioInfo != RADIOINFO::RADIOINFONOTSELECT) {

                            switch (radioInfo) {

                                case RADIOINFO::RADIOINFO1 :

                                    if (button[0] == BUTTONSTATE::LONG_PRESS) {
                                        radioInfo = RADIOINFO::RADIOINFONOTSELECT;
                                        menuCursor = 0;
                                    }

                                    break;

                                case RADIOINFO::LASTMESSAGES :

                                    if (button[0] == BUTTONSTATE::LONG_PRESS) {
                                        radioInfo = RADIOINFO::RADIOINFONOTSELECT;
                                        menuCursor = 0;
                                    }

                                    break;

                                case RADIOINFO::RADIOREQ :

                                    menuCursor = constrain(menuCursor, 1, 3);     

                                    if (button[0] == BUTTONSTATE::SHORT_PRESS && !menuCursorSelect && menuCursor == 1) {
                                        menuCursorSelect = true;
                                        menuCursorLock = true;
                                    } else if (button[0] == BUTTONSTATE::LONG_PRESS && menuCursorSelect) {
                                        menuCursorSelect = false;
                                        menuCursorLock = false;
                                    } else if (button[0] == BUTTONSTATE::LONG_PRESS && !menuCursorSelect) {
                                        radioInfo = RADIOINFO::RADIOINFONOTSELECT;
                                        menuCursor = 0;
                                        menuCursorSelect = false;
                                        menuCursorLock = false;
                                    }

                                    

                                    if (menuCursorSelect) {

                                        if (menuCursor == 1) {
                                            
                                            if (button[1] == BUTTONSTATE::SHORT_PRESS) {
                                                selectedNode--;
                                            } else if (button[2] == BUTTONSTATE::SHORT_PRESS) {
                                                selectedNode++;
                                            }

                                            if (selectedNode == 0) selectedNode = BASESTATION_ID;
                                            else if (selectedNode > BASESTATION_ID) selectedNode = 1;

                                        }

                                    } else {

                                        if (menuCursor == 2) {

                                            if (button[1] == BUTTONSTATE::LONG_PRESS || button[2] == BUTTONSTATE::LONG_PRESS) Network::requestTime(selectedNode);

                                        } else if (menuCursor == 3) {

                                            if (button[1] == BUTTONSTATE::LONG_PRESS || button[2] == BUTTONSTATE::LONG_PRESS) Network::requestSystemData(selectedNode);

                                        }

                                    }

                                    break;

                                default:
                                    break;

                            }

                        } else {

                            menuCursor = constrain(menuCursor, 1, uint8_t(RADIOINFO::RADIOEND)-1);     

                            if (button[0] == BUTTONSTATE::SHORT_PRESS) {
                                radioInfo = RADIOINFO(menuCursor);
                                menuCursor = 0;
                            } else if (button[0] == BUTTONSTATE::LONG_PRESS) {
                                startMenu = STARTMENU::STARTNOTSELECT;
                                menuCursor = 0;
                            } else if (button[1] == BUTTONSTATE::SHORT_PRESS) {
                                
                            } else if (button[1] == BUTTONSTATE::LONG_PRESS) {
                                
                            } else if (button[2] == BUTTONSTATE::SHORT_PRESS) {

                            } else if (button[2] == BUTTONSTATE::LONG_PRESS) {

                            }

                        }

                    }

                    break;

                case STARTMENU::SETTINGSMENU :
                    
                    if (button[0] == BUTTONSTATE::LONG_PRESS) {
                        startMenu = STARTMENU::STARTNOTSELECT;
                        menuCursor = 0;
                    }

                    break;

                case STARTMENU::SYSTEMINFOMENU :
                    
                    if (button[0] == BUTTONSTATE::LONG_PRESS) {
                        startMenu = STARTMENU::STARTNOTSELECT;
                        menuCursor = 0;
                    }

                    break;

                case STARTMENU::WATERINFOMENU :

                    {

                        if (waterInfo != WATERINFO::WATERINFONOTSELECT) {

                            switch (waterInfo) {

                                case WATERINFO::WATERINFO1 :

                                    if (button[0] == BUTTONSTATE::LONG_PRESS) {
                                        waterInfo = WATERINFO::WATERINFONOTSELECT;
                                        menuCursor = 0;
                                    }
                                    
                                    break;

                                case WATERINFO::UPCOMINGWATER :

                                    if (button[0] == BUTTONSTATE::LONG_PRESS) {
                                        waterInfo = WATERINFO::WATERINFONOTSELECT;
                                        menuCursor = 0;
                                    }
                                    
                                    break;

                                case WATERINFO::WATERMANUAL :

                                    menuCursor = constrain(menuCursor, 0, 7);     

                                    if (button[0] == BUTTONSTATE::SHORT_PRESS && !menuCursorSelect) {
                                        menuCursorSelect = true;
                                        menuCursorLock = true;
                                    } else if (button[0] == BUTTONSTATE::LONG_PRESS && menuCursorSelect) {
                                        menuCursorSelect = false;
                                        menuCursorLock = false;
                                    } else if (button[0] == BUTTONSTATE::LONG_PRESS && !menuCursorSelect) {
                                        startMenu = STARTMENU::STARTNOTSELECT;
                                        menuCursor = 0;
                                        menuCursorSelect = false;
                                        menuCursorLock = false;
                                    }

                                    

                                    if (menuCursorSelect) {

                                        if (button[1] == BUTTONSTATE::SHORT_PRESS) {
                                            IOControl::setValveState(menuCursor, IOVALVE_CLOSE);
                                        } else if (button[2] == BUTTONSTATE::SHORT_PRESS) {
                                            IOControl::setValveState(menuCursor, IOVALVE_OPEN);
                                        }

                                    } else {

                                        if (button[1] == BUTTONSTATE::LONG_PRESS) {
                                            IOControl::setValveState(0, IOVALVE_CLOSE);
                                            IOControl::setValveState(1, IOVALVE_CLOSE);
                                            IOControl::setValveState(2, IOVALVE_CLOSE);
                                            IOControl::setValveState(3, IOVALVE_CLOSE);
                                            IOControl::setValveState(4, IOVALVE_CLOSE);
                                            IOControl::setValveState(5, IOVALVE_CLOSE);
                                            IOControl::setValveState(6, IOVALVE_CLOSE);
                                            IOControl::setValveState(7, IOVALVE_CLOSE);
                                        } else if (button[2] == BUTTONSTATE::LONG_PRESS) {
                                            IOControl::setValveState(0, IOVALVE_OPEN);
                                            IOControl::setValveState(1, IOVALVE_OPEN);
                                            IOControl::setValveState(2, IOVALVE_OPEN);
                                            IOControl::setValveState(3, IOVALVE_OPEN);
                                            IOControl::setValveState(4, IOVALVE_OPEN);
                                            IOControl::setValveState(5, IOVALVE_OPEN);
                                            IOControl::setValveState(6, IOVALVE_OPEN);
                                            IOControl::setValveState(7, IOVALVE_OPEN);
                                        }

                                    }

                                    break;

                                case WATERINFO::WATERSETTINGS :

                                    if (button[0] == BUTTONSTATE::LONG_PRESS) {
                                        waterInfo = WATERINFO::WATERINFONOTSELECT;
                                        menuCursor = 0;
                                    }
                                    
                                    break;

                                default:
                                    break;

                            }

                        } else {

                            menuCursor = constrain(menuCursor, 1, uint8_t(WATERINFO::WATEREND)-1);     

                            if (button[0] == BUTTONSTATE::SHORT_PRESS) {
                                waterInfo = WATERINFO(menuCursor);
                                menuCursor = 0;
                            } else if (button[0] == BUTTONSTATE::LONG_PRESS) {
                                startMenu = STARTMENU::STARTNOTSELECT;
                                menuCursor = 0;
                            } else if (button[1] == BUTTONSTATE::SHORT_PRESS) {
                                
                            } else if (button[1] == BUTTONSTATE::LONG_PRESS) {
                                
                            } else if (button[2] == BUTTONSTATE::SHORT_PRESS) {

                            } else if (button[2] == BUTTONSTATE::LONG_PRESS) {

                            }

                        }

                    }



                    break;
                
                default:
                    break;
            }


        } else {


            menuCursor = constrain(menuCursor, 1, uint8_t(STARTMENU::STARTEND)-1);

            if (button[0] == BUTTONSTATE::SHORT_PRESS) {
                startMenu = STARTMENU(menuCursor);
                menuCursor = 0;
            } else if (button[0] == BUTTONSTATE::LONG_PRESS) {
                interfaceState = INTERFACESTATE::INTERFACEFRONT;
            } else if (button[1] == BUTTONSTATE::SHORT_PRESS) {
                
            } else if (button[1] == BUTTONSTATE::LONG_PRESS) {
                
            } else if (button[2] == BUTTONSTATE::SHORT_PRESS) {

            } else if (button[2] == BUTTONSTATE::LONG_PRESS) {

            }

            

        }

    }


    void menuDraw() {

        static uint32_t cursorBlinkTimestamp = 0;
        static bool cursorBlinkState = true;


        if (millis() - cursorBlinkTimestamp >= 300) {
            cursorBlinkTimestamp = millis();
            cursorBlinkState = !cursorBlinkState;
        }



        if (startMenu != STARTMENU::STARTNOTSELECT) {


            switch (startMenu) {

                case STARTMENU::MAININFOMENU :
                    
                    if (mainInfo == MAININFO::MAININFO1) {

                        display.setFont(ArialMT_Plain_10);

                        display.setTextAlignment(TEXT_ALIGN_CENTER);
                        display.drawString(DISPLAY_WIDTH/2, 0, "Haupt Info 1/2");

                        ValveTiming nextWater = WaterControl::getNextWater();

                        display.setTextAlignment(TEXT_ALIGN_LEFT);
                        display.drawString(0, 10, "Nächste Bewässerung: ");
                        display.drawString(0, 10*2, "Uhr Zeit: " + String(nextWater.hour) + ":" + String(nextWater.minute));
                        display.drawString(0, 10*3, "Länge:    " + String(nextWater.length) + " Minuten");
                        display.drawString(0, 10*4, "Ventil:   " + String(nextWater.valve));
                        display.drawString(0, 10*5, "");

                    } else {

                        display.setFont(ArialMT_Plain_10);

                        display.setTextAlignment(TEXT_ALIGN_CENTER);
                        display.drawString(DISPLAY_WIDTH/2, 0, "Haupt Info 2/2");

                        display.setTextAlignment(TEXT_ALIGN_LEFT);
                        display.drawString(0, 10, "Akku: " + String(systemStatusGlobal.vBat));
                        display.drawString(0, 10*2, "Node ID: " + String(systemStatusGlobal.nodeID));
                        display.drawString(0, 10*3, "Datum: " + String(systemStatusGlobal.now.day()) + "." + String(systemStatusGlobal.now.month()) + "." + String(systemStatusGlobal.now.year()));
                        display.drawString(0, 10*4, "Uhrzeit: " + String(systemStatusGlobal.now.hour()) + ":" + String(systemStatusGlobal.now.minute()) + ":" + String(systemStatusGlobal.now.second()));
                        display.drawString(0, 10*5, "Anzahl Starts: " + String(systemStatusGlobal.bootCount));

                    }

                    break;

                case STARTMENU::RADIOMENU :
                    
                    {

                        if (radioInfo != RADIOINFO::RADIOINFONOTSELECT) {

                            switch (radioInfo) {

                                case RADIOINFO::RADIOINFO1 :

                                    display.setTextAlignment(TEXT_ALIGN_CENTER);
                                    display.drawString(DISPLAY_WIDTH/2, 0, "Funk Info");

                                    display.setTextAlignment(TEXT_ALIGN_LEFT);
                                    display.drawString(0, 10, "Leider noch nicht");
                                    display.drawString(0, 20, "erstellt.");
                                    
                                    break;

                                case RADIOINFO::LASTMESSAGES :

                                    {
                                        display.setTextAlignment(TEXT_ALIGN_CENTER);
                                        display.drawString(DISPLAY_WIDTH/2, 0, "Letzte Nachrichten");

                                        

                                        display.setTextAlignment(TEXT_ALIGN_LEFT);
                                        display.drawString(0, 10, "ID: " + String(latestNodeTime) + " RSSI: " + String(nodeTimeRSSI[latestNodeTime]) + " SNR: " + String(nodeTimeSNR[latestNodeTime]));
                                        display.drawString(0, 20, String(nodeTime[latestNodeTime].hour()) + ":" + String(nodeTime[latestNodeTime].minute()) + ":" + String(nodeTime[latestNodeTime].second()));
                                        display.drawString(0, 30, "ID: " + String(latestNodeStatus) + " RSSI: " + String(nodeStatusRSSI[latestNodeStatus]) + " SNR: " + String(nodeStatusSNR[latestNodeStatus]));
                                        TimeSpan runTime = nodeStatus[latestNodeStatus].now - nodeStatus[latestNodeStatus].systemStartTime;
                                        display.drawString(0, 40, "V: " + String(nodeStatus[latestNodeStatus].vBat) + " P: " + String(nodeStatus[latestNodeStatus].airPressure/100,0) + " T: " + String(nodeStatus[latestNodeStatus].airTemperature,1));
                                        display.drawString(0, 50, "T+: " + String(runTime.days()) + ":" + String(runTime.hours()) + ":" + String(runTime.minutes()) + ":" + String(runTime.seconds()) + " H: " + String(nodeStatus[latestNodeStatus].airHumidity,1));

                                    }

                                    break;

                                case RADIOINFO::RADIOREQ :

                                    display.setTextAlignment(TEXT_ALIGN_CENTER);
                                    display.drawString(DISPLAY_WIDTH/2, 0, "Daten Aufrufen");

                                    display.setTextAlignment(TEXT_ALIGN_LEFT);

                                    display.drawString(10, 10, "Gewählte Node: " + String(selectedNode));
                                    display.drawString(10, 20, "Zeit Aufrufen");
                                    display.drawString(10, 30, "Daten Aufrufen");
                                    display.drawString(10, 40, "Letzte Zeit: " + String(latestNodeTime) + "," + String(nodeTimeCounter[latestNodeTime]));
                                    display.drawString(10, 50, "Letzte Daten: " + String(latestNodeStatus)  + "," + String(nodeStatusCounter[latestNodeStatus]));
                                    if (menuCursorSelect) display.drawString(0, 10*(menuCursor), cursorBlinkState ? ">" : "");
                                    else display.drawString(0, 10*(menuCursor), ">");

                                    break;

                                default:
                                    break;

                            }

                        } else {

                            display.setFont(ArialMT_Plain_10);

                            display.setTextAlignment(TEXT_ALIGN_CENTER);
                            display.drawString(DISPLAY_WIDTH/2, 0, "Funk");

                            display.setTextAlignment(TEXT_ALIGN_LEFT);
                            display.drawString(10, 10, "Funk Info");
                            display.drawString(10, 10*2, "Letzte Nachrichten");
                            display.drawString(10, 10*3, "Daten Aufrufen");
                            //display.drawString(10, 10*5, "BootCount: " + String(systemStatusGlobal.bootCount));

                            display.drawString(0, 10*(menuCursor), ">");

                        }

                    }

                    break;

                case STARTMENU::SETTINGSMENU :
                    


                    break;

                case STARTMENU::SYSTEMINFOMENU :
                    


                    break;

                case STARTMENU::WATERINFOMENU :

                    
                    if (waterInfo != WATERINFO::WATERINFONOTSELECT) {

                        switch (waterInfo) {

                            case WATERINFO::WATERINFO1 :

                                display.setTextAlignment(TEXT_ALIGN_CENTER);
                                display.drawString(DISPLAY_WIDTH/2, 0, "Wasser Info");

                                display.setTextAlignment(TEXT_ALIGN_LEFT);
                                display.drawString(0, 10, "Leider noch nicht");
                                display.drawString(0, 20, "erstellt.");
                                
                                break;

                            case WATERINFO::UPCOMINGWATER :

                                {
                                    display.setTextAlignment(TEXT_ALIGN_CENTER);
                                    display.drawString(DISPLAY_WIDTH/2, 0, "Nächste Bewässerungen");

                                    ValveTiming nextWater = WaterControl::getNextWater();

                                    display.setTextAlignment(TEXT_ALIGN_LEFT);
                                    display.drawString(0, 10, String(nextWater.hour) + ":" + String(nextWater.minute) + " " + String(nextWater.minute) + " min " + String(nextWater.valve));
                                }

                                break;

                            case WATERINFO::WATERMANUAL :

                                display.setTextAlignment(TEXT_ALIGN_CENTER);
                                display.drawString(DISPLAY_WIDTH/2, 0, "Manuell Steuerung");

                                display.setTextAlignment(TEXT_ALIGN_LEFT);

                                if (menuCursor < 5) {
                                    display.drawString(10, 10, "Ventil 1: " + String(IOControl::getValveState(0) == IOVALVE_OPEN ? "Offen" : "Zu"));
                                    display.drawString(10, 20, "Ventil 2: " + String(IOControl::getValveState(1) == IOVALVE_OPEN ? "Offen" : "Zu"));
                                    display.drawString(10, 30, "Ventil 3: " + String(IOControl::getValveState(2) == IOVALVE_OPEN ? "Offen" : "Zu"));
                                    display.drawString(10, 40, "Ventil 4: " + String(IOControl::getValveState(3) == IOVALVE_OPEN ? "Offen" : "Zu"));
                                    display.drawString(10, 50, "Ventil 5: " + String(IOControl::getValveState(4) == IOVALVE_OPEN ? "Offen" : "Zu"));
                                    if (menuCursorSelect) display.drawString(0, 10*(menuCursor + 1), cursorBlinkState ? ">" : "");
                                    else display.drawString(0, 10*(menuCursor + 1), ">");
                                } else {
                                    display.drawString(10, 10, "Ventil 6: " + String(IOControl::getValveState(5) == IOVALVE_OPEN ? "Offen" : "Zu"));
                                    display.drawString(10, 20, "Ventil 7: " + String(IOControl::getValveState(6) == IOVALVE_OPEN ? "Offen" : "Zu"));
                                    display.drawString(10, 30, "Ventil 8: " + String(IOControl::getValveState(8) == IOVALVE_OPEN ? "Offen" : "Zu"));
                                    if (menuCursorSelect) display.drawString(0, 10*(menuCursor-5 + 1), cursorBlinkState ? ">" : "");
                                    else display.drawString(0, 10*(menuCursor-5 + 1), ">");
                                }

                                break;

                            case WATERINFO::WATERSETTINGS :

                                display.setTextAlignment(TEXT_ALIGN_CENTER);
                                display.drawString(DISPLAY_WIDTH/2, 0, "Einstellungen");

                                display.setTextAlignment(TEXT_ALIGN_LEFT);
                                display.drawString(0, 10, "Leider noch nicht");
                                display.drawString(0, 20, "erstellt.");
                                
                                break;

                            default:
                                break;

                        }

                    } else {

                        display.setFont(ArialMT_Plain_10);

                        display.setTextAlignment(TEXT_ALIGN_CENTER);
                        display.drawString(DISPLAY_WIDTH/2, 0, "Wasser");

                        display.setTextAlignment(TEXT_ALIGN_LEFT);
                        display.drawString(10, 10, "Wasser Info");
                        display.drawString(10, 10*2, "Nächste Bewässerungen");
                        display.drawString(10, 10*3, "Manuell Steuerung");
                        display.drawString(10, 10*4, "Einstellungen");
                        //display.drawString(10, 10*5, "BootCount: " + String(systemStatusGlobal.bootCount));

                        display.drawString(0, 10*(menuCursor), ">");

                    }

                    break;
                
                default:
                    break;
            }


        } else {


            display.setFont(ArialMT_Plain_10);

            display.setTextAlignment(TEXT_ALIGN_CENTER);
            display.drawString(DISPLAY_WIDTH/2, 0, "HauptMenü");

            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.drawString(10, 10, "Haupt Info");
            display.drawString(10, 10*2, "Wasser");
            display.drawString(10, 10*3, "System");
            display.drawString(10, 10*4, "Funk");
            display.drawString(10, 10*5, "Einstellungen");
            //display.drawString(10, 10*5, "BootCount: " + String(systemStatusGlobal.bootCount));

            display.drawString(0, 10*(menuCursor), ">");
            

        }

    }


    void menuDisplay() {

        interfaceAutoOffTimeout = INTERFACE_MAIN_TIMEOUT;

        menuNav();

        menuDraw();

        /*static uint32_t cursorBlinkTimestamp = 0;
        static bool cursorBlinkState = true;

        interfaceAutoOffTimeout = INTERFACE_MAIN_TIMEOUT;

        if (button[0] == BUTTONSTATE::SHORT_PRESS) menuCursorSelect = true;
        else if (button[0] == BUTTONSTATE::LONG_PRESS) {
            menuCursorSelect = false;
            if (menuCursor == 4) setEEPROMBootCount(systemStatusGlobal.bootCount);
        }

        if (button[1] == BUTTONSTATE::SHORT_PRESS && menuCursor > 0 && !menuCursorSelect) menuCursor--;
        else if (button[2] == BUTTONSTATE::SHORT_PRESS && menuCursor < 4 && !menuCursorSelect) menuCursor++;

        display.setFont(ArialMT_Plain_10);

        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(DISPLAY_WIDTH/2, 0, "HauptMenü");

        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(10, 10, "Wasser");
        display.drawString(10, 10*2, "System");
        display.drawString(10, 10*3, "Funk");
        display.drawString(10, 10*4, "Einstellungen");
        display.drawString(10, 10*5, "BootCount: " + String(systemStatusGlobal.bootCount));

        if (menuCursorSelect && millis() - cursorBlinkTimestamp >= 300) {
            cursorBlinkTimestamp = millis();
            cursorBlinkState = !cursorBlinkState;
        } else if (!menuCursorSelect) {
            cursorBlinkState = true;
            cursorBlinkTimestamp = millis();
        }
        
        display.drawString(0, 10*(menuCursor+1), cursorBlinkState ? ">":"");

        if (menuCursor == 4 && menuCursorSelect && button[1] == BUTTONSTATE::SHORT_PRESS && systemStatusGlobal.bootCount > 0) systemStatusGlobal.bootCount--;
        if (menuCursor == 4 && menuCursorSelect && button[2] == BUTTONSTATE::SHORT_PRESS) systemStatusGlobal.bootCount++;*/
    }


    void frontDisplay() {

        static uint32_t lastCodeUpdate = millis();
        static uint8_t codeButton = 0;

        interfaceAutoOffTimeout = INTERFACE_FRONT_TIMEOUT;

        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(DISPLAY_WIDTH/2, 0, "NodeID: " + String(systemStatusGlobal.nodeID));
        display.drawString(DISPLAY_WIDTH/2, 10, "VBat: " + String(constrain((systemStatusGlobal.vBat-3.2)*100, 0, 100),0) + "%");
        TimeSpan timeLength = systemStatusGlobal.now - systemStatusGlobal.systemStartTime;
        display.drawString(DISPLAY_WIDTH/2, 20, "Laufzeit: " + String(timeLength.seconds()) + "s, " + String(timeLength.minutes()) + "m, " + String(timeLength.hours()) + "h, " + String(timeLength.days()) + "t");

        if (button[codeButton] == BUTTONSTATE::SHORT_PRESS && button[(codeButton+1)%3] == BUTTONSTATE::NO_PRESS && button[(codeButton+2)%3] == BUTTONSTATE::NO_PRESS) {
            frontCodeCorrectCount++;
            uint8_t newNum = random(3);
            while (newNum == codeButton) newNum = random(3);
            codeButton = newNum;

        } else if (button[0] != BUTTONSTATE::NO_PRESS || button[1] != BUTTONSTATE::NO_PRESS || button[2] != BUTTONSTATE::NO_PRESS) frontCodeCorrectCount = 0;

        display.drawString(DISPLAY_WIDTH/2, 16*2, String(frontCodeCorrectCount) + " von " + String(FRONT_CODE_COUNT_NUM));

        if (frontCodeCorrectCount >= FRONT_CODE_COUNT_NUM)  {
            interfaceState = INTERFACESTATE::INTERFACEACTIVE;
            return;
        }

        if (codeButton == 0) {
            display.fillCircle(10, 55, 5);
            display.drawCircle(DISPLAY_WIDTH/2, 55, 5);
            display.drawCircle(DISPLAY_WIDTH-10, 55, 5);
        } else if (codeButton == 1) {
            display.drawCircle(10, 55, 5);
            display.fillCircle(DISPLAY_WIDTH/2, 55, 5);
            display.drawCircle(DISPLAY_WIDTH-10, 55, 5);
        } else if (codeButton == 2) {
            display.drawCircle(10, 55, 5);
            display.drawCircle(DISPLAY_WIDTH/2, 55, 5);
            display.fillCircle(DISPLAY_WIDTH-10, 55, 5);
        }

        



    }


    void loop(SystemStatus _systemStatus) {
        //systemStatus = _systemStatus;


        if (!displayRunning) { // Attempt to get display running

            if (displayStartRetries < 5 && millis() - lastDisplayStartAttempt >= 500) { // Attempt start every 500ms and max 5 times
                lastDisplayStartAttempt = millis();

                if (display.init()) {

                    display.clear();
                    display.setBrightness(255);
                    display.display();

                    displayRunning = true;

                }

            } else return;

        }


        if (millis() - lastPinRead >= PIN_READ_RATE_MS) { // Read button status
            lastPinRead = millis();

            float val1 = touchRead(_touchPin1);
            float val2 = touchRead(_touchPin2);
            float val3 = touchRead(_touchPin3);

            if (interfaceState != INTERFACESTATE::INTERFACEOFF) {
                _touchValAVG[0] = _touchValAVG[0]*PIN_AVG_LPF_FACTOR + val1*(1-PIN_AVG_LPF_FACTOR);
                _touchValAVG[1] = _touchValAVG[1]*PIN_AVG_LPF_FACTOR + val2*(1-PIN_AVG_LPF_FACTOR);
                _touchValAVG[2] = _touchValAVG[2]*PIN_AVG_LPF_FACTOR + val3*(1-PIN_AVG_LPF_FACTOR);
            } else {
                _touchValAVG[0] = _touchValAVG[0]*PIN_AVG_LPF_FACTOR + val1*(1-PIN_AVG_LPF_FACTOR);
                _touchValAVG[1] = _touchValAVG[1]*PIN_AVG_LPF_FACTOR + val2*(1-PIN_AVG_LPF_FACTOR);
                _touchValAVG[2] = _touchValAVG[2]*PIN_AVG_LPF_FACTOR + val3*(1-PIN_AVG_LPF_FACTOR);
            }

            _touchVal1[0] = _touchValAVG[0] - val1;
            _touchVal2[0] = _touchValAVG[1] - val2;
            _touchVal3[0] = _touchValAVG[2] - val3;


            touchBool[0] = (_touchVal1[0] > PIN_ACTIVE_THRESHOLD) && (_touchVal1[1] > PIN_ACTIVE_THRESHOLD) && (_touchVal1[2] > PIN_ACTIVE_THRESHOLD);
            touchBool[1] = (_touchVal2[0] > PIN_ACTIVE_THRESHOLD) && (_touchVal2[1] > PIN_ACTIVE_THRESHOLD) && (_touchVal2[2] > PIN_ACTIVE_THRESHOLD);
            touchBool[2] = (_touchVal3[0] > PIN_ACTIVE_THRESHOLD) && (_touchVal3[1] > PIN_ACTIVE_THRESHOLD) && (_touchVal3[2] > PIN_ACTIVE_THRESHOLD);

            /*Serial.println();
            Serial.println("AVG: " + String(_touchValAVG[0]) + " " + String(_touchValAVG[1]) + " " + String(_touchValAVG[2]));
            Serial.println("Val: " + String(_touchVal1[0]) + " " + String(_touchVal2[0]) + " " + String(_touchVal3[0]));
            Serial.println("Touch: " + String(touchBool[0]) + " " + String(touchBool[1]) + " " + String(touchBool[2]));*/

            for (int i = 0; i < 2; i++) {
                _touchVal1[i+1] = _touchVal1[i];
                _touchVal2[i+1] = _touchVal2[i];
                _touchVal3[i+1] = _touchVal3[i];
            }

        }



        if (millis() - lastInterfaceUpdate >= INTERFACE_UPDATE_RATE) { //Update interface
            lastInterfaceUpdate = millis();


            for (int i = 0; i < 3; i++) { //Find out what is going on with the buttons

                if (touchBool[i] && buttonPressStage[i] == 0) {
                    buttonPressStage[i] = 1;
                    buttonPressTimestamp[i] = millis();
                } else if ((buttonPressStage[i] == 1 || buttonPressStage[i] == 2) && touchBool[i] && millis() - buttonPressTimestamp[i] >= LONG_PRESS_THRESHOLD) {
                    if (buttonPressStage[i] != 2) button[i] = BUTTONSTATE::LONG_PRESS;
                    else button[i] = BUTTONSTATE::HOLD_PRESS;
                    buttonPressStage[i] = 2;
                    //Serial.println("Button " + String(i+1) + " Long");
                } else if (buttonPressStage[i] == 1 && !touchBool[i]) {
                    buttonPressStage[i] = 0;
                    button[i] = BUTTONSTATE::SHORT_PRESS;
                    //Serial.println("Button " + String(i+1) + " Short");
                } else if (buttonPressStage[i] == 2 && !touchBool[i]) {
                    buttonPressStage[i] = 0;
                    button[i] = BUTTONSTATE::NO_PRESS;
                }

                if (button[i] != BUTTONSTATE::NO_PRESS) lastInterfaceInterrupt = millis();

            }

            /*if (button[0] == BUTTONSTATE::SHORT_PRESS) Serial.println("Short");
            if (button[0] == BUTTONSTATE::LONG_PRESS) Serial.println("Long");
            if (button[0] == BUTTONSTATE::HOLD_PRESS) Serial.println("Hold");*/


            if (button[0] != BUTTONSTATE::NO_PRESS || button[1] != BUTTONSTATE::NO_PRESS || button[2] != BUTTONSTATE::NO_PRESS) lastInterfaceInterrupt = millis();


            if (interfaceState == INTERFACESTATE::INTERFACEOFF && button[0] == BUTTONSTATE::NO_PRESS && button[1] == BUTTONSTATE::LONG_PRESS && button[2] == BUTTONSTATE::LONG_PRESS) interfaceState = INTERFACESTATE::INTERFACEFRONT;
            else if (interfaceState == INTERFACESTATE::INTERFACEFRONT && button[0] == BUTTONSTATE::NO_PRESS && button[1] == BUTTONSTATE::LONG_PRESS && button[2] == BUTTONSTATE::LONG_PRESS || millis() - lastInterfaceInterrupt >= interfaceAutoOffTimeout) interfaceState = INTERFACESTATE::INTERFACEOFF;



            if (interfaceState == INTERFACESTATE::INTERFACEACTIVE) {

                display.displayOn();
                display.clear();

                menuDisplay();

                display.display();
                frontCodeCorrectCount = 0;

            } else if (interfaceState == INTERFACESTATE::INTERFACEFRONT) {
                
                display.displayOn();
                display.clear();

                frontDisplay();

                display.display();
                
                menuCursor = 0;
                startMenu = STARTMENU::STARTNOTSELECT;
                mainInfo = MAININFO::MAININFONOTSELECT;
            
            } else {

                display.displayOff();
                display.display();



                menuCursor = 0;
                frontCodeCorrectCount = 0;
                startMenu = STARTMENU::STARTNOTSELECT;
                mainInfo = MAININFO::MAININFONOTSELECT;

            }

            button[0] = BUTTONSTATE::NO_PRESS;
            button[1] = BUTTONSTATE::NO_PRESS;
            button[2] = BUTTONSTATE::NO_PRESS;

        }


    }
    


}



#include "utils/network/network.h"




namespace Network {
    

    namespace {

        enum PACKETTYPE {
            NETWORK_JOIN,
            NETWORK_HEARTBEAT,
            NETWORK_SLEEPING,
            NETWORK_TIME_REQ,
            NETWORK_TIME_DATA, 
            NETWORK_WATERINGINFO_REQ,
            NETWORK_WATERINGINFO_DATA,
            NETWORK_NODEDATA_REQ,
            NETWORK_NODEDATA_DATA
        };


        RadioComm _radio;

        bool waitingForPackets = false;

        DateTime waitForPacketsStart = DateTime(0U);

        bool sleeping = false;

        bool powerOff = false;

        bool receiverOff = false;

    }



    void sleep() {sleeping = true; powerOff = false;}
    void wake() {sleeping = false; powerOff = false;}
    void shutdown() {powerOff = true;}
    bool busy() {return _radio.getBusy();}



    void setup() {


        if (systemStatusGlobal.eepromStatus == DEVICESTATUS::RUNNING && _radio.begin(systemStatusGlobal.nodeID, NSS, NRESET, RFBUSY, DIO1, DEVICE_SX1280, RXEN, TXEN)) {

            systemStatusGlobal.networkStatus = DEVICESTATUS::RUNNING;

        } else {

            systemStatusGlobal.networkStatus = DEVICESTATUS::FAILURE;

        }

        _radio.startReceive();

    }

    
    void broadcastSleep() {

    }


    void broadcastWake() {

    }


    void loop() {


        _radio.loop();


        if (systemStatusGlobal.now.second()%10 == 0 && !powerOff && !waitingForPackets) waitingForPackets = true;
        else if (systemStatusGlobal.now.second()%10 > 2 && waitingForPackets) waitingForPackets = false;


        if ((waitingForPackets || !sleeping) && !powerOff && receiverOff) {

            _radio.startReceive();
            receiverOff = false;

        } else if (((!waitingForPackets && sleeping) || powerOff) && !receiverOff) {

            _radio.stopReceive();
            receiverOff = true;

        }



        if (_radio.available()) {

            Serial.println("PACKET RECEIVED!");

            uint8_t size = _radio.receivedPacketSize();
            uint8_t nodeID = _radio.receivedPacketNodeID();
            uint8_t rssi = _radio.receivedPacketRSSI();
            uint8_t snr = _radio.receivedPacketSNR();

            PACKETTYPE packetType = PACKETTYPE(_radio.readByte(0));

            if (packetType == PACKETTYPE::NETWORK_TIME_DATA) {

                DateTime buf;

                _radio.readPacket(&buf, sizeof(buf), 1);

                latestNodeTime = nodeID;
                nodeTime[latestNodeTime] = buf;
                nodeTimeRSSI[latestNodeTime] = rssi;
                nodeTimeSNR[latestNodeTime] = snr;
                nodeTimeCounter[latestNodeTime]++;


            } else if (packetType == PACKETTYPE::NETWORK_NODEDATA_DATA) {

                SystemStatus buf;

                _radio.readPacket(&buf, sizeof(buf), 1);

                latestNodeStatus = nodeID;
                nodeStatus[latestNodeStatus] = buf;
                nodeStatusRSSI[latestNodeStatus] = rssi;
                nodeStatusSNR[latestNodeStatus] = snr;
                nodeStatusCounter[latestNodeStatus]++;


            } else if (packetType == PACKETTYPE::NETWORK_TIME_REQ) {

                timeSend(systemStatusGlobal.now, nodeID);

            } else if (packetType == PACKETTYPE::NETWORK_NODEDATA_REQ) {

                systemDataSend(systemStatusGlobal, nodeID);

            }

            timeSend(systemStatusGlobal.now, nodeID);

        }

    }



    bool requestTime(uint8_t node) {

        if (sleeping || powerOff || node > BASESTATION_ID) return false;

        PACKETTYPE reqBuf = PACKETTYPE::NETWORK_TIME_REQ;

        _radio.writePacket(&reqBuf, sizeof(reqBuf), NETWORK_TXPOWER, node, NONLOSSY_REQUEST, 1500, 5);

    }



    bool requestSystemData(uint8_t node) {

        if (sleeping || powerOff || node > BASESTATION_ID) return false;

        PACKETTYPE reqBuf = PACKETTYPE::NETWORK_NODEDATA_REQ;

        _radio.writePacket(&reqBuf, sizeof(reqBuf), NETWORK_TXPOWER, node, NONLOSSY_REQUEST, 1500, 5);

    }



    bool timeSend(DateTime time, uint8_t node) {

        //if (_radio.getBusy()) return false;

        if (sleeping || powerOff) return false;

        _radio.addByteAsHeaderToPacket(PACKETTYPE::NETWORK_TIME_DATA);

        if (node != NODE_BROADCAST) _radio.writePacket(&time, sizeof(time), NETWORK_TXPOWER, node, NONLOSSY_SEND, 1500, 5);
        else _radio.writePacket(&time, sizeof(time), NETWORK_TXPOWER, BROADCAST_ADDRESS);

    }



    bool systemDataSend(SystemStatus status, uint8_t node) {

        if (sleeping || powerOff) return false;

        _radio.addByteAsHeaderToPacket(PACKETTYPE::NETWORK_NODEDATA_DATA);

        if (node != NODE_BROADCAST) _radio.writePacket(&status, sizeof(status), NETWORK_TXPOWER, node, NONLOSSY_SEND, 1500, 5);
        else _radio.writePacket(&status, sizeof(status), NETWORK_TXPOWER, BROADCAST_ADDRESS);
        
    }


}
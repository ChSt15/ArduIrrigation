#ifndef _COMMSLIB_H_
#define _COMMSLIB_H_

#include <Arduino.h>
#include <SPI.h>
#include "SX128XLT.h"
#include <utils/QueueData/queueData.h>


#define PACKET_HEADER_1 0xAA
#define PACKET_HEADER_2 0xFF
#define PACKET_END 0xAF

#define MAX_PAYLOAD_SIZE 200
#define GENERAL_BUFFER_SIZE 255

#define MAX_LOOPRATE 100

#define BROADCAST_ADDRESS 255

#define DEBUGRADIO

//#define PAYLOAD_START 7 //first byte of payload



class RadioComm {
public:

    enum PAYLOAD_TYPE {
        PAYLOAD_JOIN,
        PAYLOAD_ACK,
        PAYLOAD_RADIOSETTING,
        PAYLOAD_GENERIC_DATA = 255
    };


    enum SEND_STATUS {
        SENDSTATUS_SENDING,
        SENDSTATUS_SUCCESS,
        SENDSTATUS_FAILED,
        SENDSTATUS_TIMEDOUT
    };


    enum DECODE_STAGE {
        DECODE_READY,
        DECODE_HEADER1,
        DECODE_HEADER2,
        DECODE_LENGTH,
        DECODE_PACKETS,
        DECODE_SEQUENCE,
        DECODE_PAYLOADSEQUENCE,
        DECODE_SYSTEMID,
        DECODE_DESTINATIONID,
        DECODE_PAYLOADTYPE,
        DECODE_PAYLOAD,
        DECODE_CHECKSUM,
        DECODE_ENDING
    };


    struct PacketStruct {

        uint8_t payloadLength = 0;
        uint8_t packets = 0;
        uint8_t sequence = 0;
        uint8_t payloadSequence = 0;
        uint8_t systemID = 0;
        uint8_t destinationID = 0;
        uint8_t returnAck = false;
        uint8_t payloadType;
        uint8_t payload[MAX_PAYLOAD_SIZE];
        uint8_t checkSum;

        uint8_t getCheckSum() {
            uint8_t sum = (uint8_t)PACKET_HEADER_1 + PACKET_HEADER_2 + uint8_t(payloadLength) + sequence + payloadSequence + systemID + destinationID + returnAck + payloadType;
            for (int i = 0; i < payloadLength && i < MAX_PAYLOAD_SIZE; i++) {
                sum += payload[i];
            }
            return sum;
        }

        //FOLLOWING VARIABLES ARE NOT TO BE SENT, BUT RATHER TO HOLD DATA WHEN PACKET IS RECEIVED AND ARE ONLY ABOUT THE RECEIVER NOT TRANSMITTER

        int8_t RSSI; 
        int8_t SNR;
        uint8_t power = 10;

    };


    struct radioNode {
        uint8_t systemID = 0;
        uint8_t sequence = 0;
        uint8_t payloadSequence = 0;
    };


    struct radioSettingStruct {
        uint32_t frequency = 2445000000;
        uint8_t bandwidth = LORA_BW_0800;
        uint8_t spreadingFactor = LORA_SF12; //LORA_SF12;
        uint8_t codeRate = LORA_CR_4_6;
        uint8_t powerLow = 10;
        uint8_t powerHigh = 10;
    };
    

    RadioComm();

    bool available();
    uint8_t receivedPacketNodeID(); //returns systemID of sender
    uint8_t receivedPacketRSSI();
    uint8_t receivedPacketSNR();
    uint8_t receivedPacketSize();

    bool readPacket(void *payload, uint16_t size, uint16_t startByte = 0);
    uint8_t readByte(uint16_t index, bool markPacketAsRead = true);

    bool writePacket(void *payload, uint16_t size, uint8_t power = 0, uint8_t destinationID = BROADCAST_ADDRESS, bool ackReturn = false, uint32_t timeout = 0, uint8_t resendAttempts = 0);
    void addByteAsHeaderToPacket(uint8_t byte);

    SEND_STATUS getSendStatus() {return _packetSendStatus;}

    void setSystemID(uint8_t systemID) {_selfNode.systemID = systemID;}

    bool getBusy() {return _commsBusy() ||_sending;}
    uint32_t getNodeFailedSendCounter() {return _nodeFailedSendCounter;}

    void startReceive();
    bool stopReceive();

    SX128XLT* getRadio() {return &_radio;}
    radioSettingStruct* getSetting() {return &_selfSetting;}

    void updateSettings();

    bool begin(uint8_t systemID, int NSSPin, int NRESETPin, int RFBUSYPin, int DIO1Pin, uint8_t LORA_DEVICE, int RXENPin = -1, int TXENPin = -1); //Setup

    void loop();



//protected:


private:

    SX128XLT _radio;
    int _DIO1Pin;

    radioNode _selfNode;

    radioSettingStruct _selfSetting;

    uint32_t _nodeFailedSendCounter;
    uint32_t _nodeTimeout;
    uint8_t _nodeResendAttempts;
    uint32_t _nodeResendAttemptTimestamp;
    uint32_t _nodeCommsStartTimestamp;
    PacketStruct _activeNodePacketBuffer;
    uint8_t _nodeLastSequence[255];
    uint8_t _buffer[GENERAL_BUFFER_SIZE];

    PacketStruct _receivedPacket;
    PacketStruct _sendPacket;
    PacketStruct _ackPacket;

    uint32_t _lastLoopTimestamp = 0;

    bool _receiving = false;
    bool _sending = false;
    bool _sleeping = false;

    bool _packetReceived = false;
    bool _sendAck = false;

    bool _packetHeaderAttached = false;

    SEND_STATUS _packetSendStatus;


    void _sendACK(uint8_t destinationID);
    bool _commsBusy();

};



#endif

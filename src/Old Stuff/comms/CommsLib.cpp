#include "CommsLib.h"

#include <Arduino.h>


RadioComm::RadioComm() { //initialize queues

    

}



void RadioComm::updateSettings() {

    if (_sleeping) _radio.wake();

    _radio.setupLoRa(_selfSetting.frequency, 0, _selfSetting.spreadingFactor, _selfSetting.bandwidth, _selfSetting.codeRate);
    
    if (_receiving) {

        _radio.receive(_buffer, GENERAL_BUFFER_SIZE, 0, NO_WAIT); // put radio back into receive mode

    } else if (_sleeping) {

        _radio.setSleep(CONFIGURATION_RETENTION);

    }

}



bool RadioComm::begin(uint8_t systemID, int NSSPin, int NRESETPin, int RFBUSYPin, int DIO1Pin, uint8_t LORA_DEVICE, int RXENPin, int TXENPin) {

    _DIO1Pin = DIO1Pin;
    _selfNode.systemID = systemID;

    _radio.resetDevice();

    if (!_radio.begin(NSSPin, NRESETPin, RFBUSYPin, DIO1Pin, RXENPin, TXENPin, LORA_DEVICE)) {

        #ifdef DEBUGRADIO
            Serial.println("RADIO FAILED TO START!");
        #endif

        return false;
    }

    #ifdef DEBUGRADIO
        Serial.println("Radio start success.");
    #endif

    _radio.setupLoRa(_selfSetting.frequency, 0, _selfSetting.spreadingFactor, _selfSetting.bandwidth, _selfSetting.codeRate);

    _radio.setDioIrqParams(IRQ_RADIO_ALL, IRQ_RADIO_ALL, 0, 0);

    return true;

}



void RadioComm::startReceive() {

    _receiving = true;

    #ifdef DEBUGRADIO 
        Serial.print("Starting Receive mode");
    #endif


    if (_sleeping) { // wake radio if asleep
        _sleeping = false;
        _radio.wake();
        _radio.receive(_buffer, GENERAL_BUFFER_SIZE, 0, NO_WAIT);
    }

    

}



bool RadioComm::stopReceive() { 

    bool success = false;

    #ifdef DEBUGRADIO 
        Serial.print("Exiting Receive mode");
    #endif

    _receiving = false;

    if (!_sending && _packetSendStatus != SEND_STATUS::SENDSTATUS_SENDING) {

        _radio.setSleep(CONFIGURATION_RETENTION);
        success = true;

    }

    return success;

}



uint8_t RadioComm::receivedPacketNodeID() {
    
    return _receivedPacket.systemID;

}



uint8_t RadioComm::receivedPacketSize() {
    
    return _receivedPacket.payloadLength;

}



uint8_t RadioComm::receivedPacketRSSI() {
    
    return _receivedPacket.RSSI;

}



uint8_t RadioComm::receivedPacketSNR() {
    
    return _receivedPacket.SNR;

}



bool RadioComm::available() {

    loop();
    
    return _packetReceived;

}



bool RadioComm::readPacket(void *payload, uint16_t size, uint16_t startByte) {

    int32_t maxSize = (int32_t)_receivedPacket.payloadLength - startByte;

    memcpy(payload, _receivedPacket.payload + startByte, constrain(size, 0, max(maxSize, 0)));

    _packetReceived = false;

    return true;

}



uint8_t RadioComm::readByte(uint16_t index, bool markPacketAsRead) {

    if (markPacketAsRead) _packetReceived = false;

    return _receivedPacket.payload[min(index, uint16_t(_receivedPacket.payloadLength))];

}


void RadioComm::addByteAsHeaderToPacket(uint8_t byte) {

    _packetHeaderAttached = true;

    _sendPacket.payload[0] = byte;

}


bool RadioComm::writePacket(void *payload, uint16_t size, uint8_t power, uint8_t destinationID, bool ackReturn, uint32_t timeout, uint8_t resendAttempts) {

    bool sendSuccess = false;

    if (!_commsBusy()) { //Check is we are alrady sending a packet and therefore busy

        uint8_t payloadSize = min(uint16_t(MAX_PAYLOAD_SIZE - (_packetHeaderAttached ? 1:0)) , size);

        if (!_packetHeaderAttached) memcpy(_sendPacket.payload, payload, payloadSize);
        else memcpy(_sendPacket.payload+1, payload, payloadSize);

        _sendPacket.destinationID = destinationID;
        _sendPacket.packets = 0;
        _sendPacket.payloadLength = payloadSize;
        _sendPacket.payloadSequence = 0;
        _sendPacket.payloadType = PAYLOAD_TYPE::PAYLOAD_GENERIC_DATA;
        _sendPacket.sequence++;
        _sendPacket.systemID = _selfNode.systemID;
        _sendPacket.returnAck = ackReturn;
        _sendPacket.checkSum = _sendPacket.getCheckSum();

        _sendPacket.power = power == 0 ? _selfSetting.powerLow:power;

        _packetSendStatus = SEND_STATUS::SENDSTATUS_SENDING;
        _nodeResendAttempts = resendAttempts;
        _nodeTimeout = timeout;
        _nodeResendAttemptTimestamp = 0;
        _nodeCommsStartTimestamp = millis();
        _nodeFailedSendCounter = 0;

        sendSuccess = true;
        _packetHeaderAttached = false;

    }

    loop();

    return sendSuccess;

}



void RadioComm::_sendACK(uint8_t destinationID) {

    if (true) { //Temporary. Cousld probably be changed

        PacketStruct packetBuffer;

        packetBuffer.destinationID = destinationID;
        packetBuffer.packets = 0;
        packetBuffer.payloadLength = 0;
        packetBuffer.payloadSequence = 0;
        packetBuffer.payloadType = PAYLOAD_TYPE::PAYLOAD_ACK;
        packetBuffer.sequence = 0;
        packetBuffer.systemID = _selfNode.systemID;
        packetBuffer.checkSum = packetBuffer.getCheckSum();

        packetBuffer.power = _selfSetting.powerHigh;

        _ackPacket = packetBuffer;

        _sendAck = true;

    }

}



bool RadioComm::_commsBusy() {

    if (_packetSendStatus == SEND_STATUS::SENDSTATUS_SENDING) return true;

    return false;

}



void RadioComm::loop() {


    if (digitalRead(_DIO1Pin)) { //Something triggered interrupt

        #ifdef DEBUGRADIO
            Serial.println("Radio triggered interrupt");
        #endif

        

        uint16_t irqStatus = _radio.readIrqStatus();


        if (irqStatus & (IRQ_RX_DONE + IRQ_HEADER_VALID)) { // If interrupt says data good then get data

            #ifdef DEBUGRADIO
                Serial.println("Interrupt says data received");
            #endif

            //byte packetL = _radio.receive(_buffer, GENERAL_BUFFER_SIZE, 100, WAIT_RX);
            //_radio.

            byte packetL = _radio.readRXPacketL();

            if (packetL != 0) { // make sure packet is okay

                int8_t SNR = _radio.readPacketSNR();
                int8_t RSSI = _radio.readPacketRSSI();

                _radio.startReadSXBuffer(0);

                int i;

                for (i = 0; i < packetL && i < GENERAL_BUFFER_SIZE; i++) _buffer[i] = _radio.readUint8();

                //for (; i < GENERAL_BUFFER_SIZE; i++) _buffer[i] = 0;

                _radio.endReadSXBuffer();

                #ifdef DEBUGRADIO
                    Serial.println("Data Received and is " + String(packetL) + " bytes long.");
                #endif

                PacketStruct packetBuffer;
                bool checksPass = false;

                packetBuffer.RSSI = _radio.readPacketRSSI();
                packetBuffer.SNR = _radio.readPacketSNR();

                if (_buffer[0] == PACKET_HEADER_1 && _buffer[1] == PACKET_HEADER_2) {

                    

                    packetBuffer.payloadLength = _buffer[2];
                    packetBuffer.packets = _buffer[3];
                    packetBuffer.sequence = _buffer[4];
                    packetBuffer.payloadSequence = _buffer[5];
                    packetBuffer.systemID = _buffer[6];
                    packetBuffer.destinationID = _buffer[7];
                    packetBuffer.returnAck = _buffer[8];
                    packetBuffer.payloadType = _buffer[9];

                    packetBuffer.SNR = SNR;
                    packetBuffer.RSSI = RSSI;

                    #ifdef DEBUGRADIO
                        Serial.println("Data Payload is " + String(packetBuffer.payloadLength) + " bytes long.");
                    #endif
                    
                    for (int i = 0; i < packetBuffer.payloadLength && i < MAX_PAYLOAD_SIZE; i++) {
                        packetBuffer.payload[i] = _buffer[10+i];
                    }

                    packetBuffer.checkSum = _buffer[packetBuffer.payloadLength + 10];

                    if (_buffer[packetBuffer.payloadLength + 11] == PACKET_END && packetBuffer.systemID != _selfNode.systemID && packetBuffer.checkSum == packetBuffer.getCheckSum()) checksPass = true;

                }

                if (checksPass) {


                    if (packetBuffer.payloadType == PAYLOAD_TYPE::PAYLOAD_GENERIC_DATA && (packetBuffer.destinationID == BROADCAST_ADDRESS || packetBuffer.destinationID == _selfNode.systemID)) {


                        #ifdef DEBUGRADIO
                            Serial.println("Received Data had a Generic type payload");
                        #endif


                        if (packetBuffer.destinationID != BROADCAST_ADDRESS && packetBuffer.returnAck) {

                            #ifdef DEBUGRADIO
                                Serial.println("Transmitter " + String(packetBuffer.systemID) + " requires ACK. Sending ACK...");
                            #endif

                            _sendACK(packetBuffer.systemID);

                        }

                        if (packetBuffer.sequence != _nodeLastSequence[packetBuffer.systemID]) {
                            _nodeLastSequence[packetBuffer.systemID] = packetBuffer.sequence;

                            #ifdef DEBUGRADIO
                                Serial.println("Data was new. Placing in queue...");
                            #endif
                            
                            _receivedPacket = packetBuffer;
                            _packetReceived = true;

                        } else {

                            #ifdef DEBUGRADIO
                                Serial.println("Data was a resend (No sequence change). Ack was sent back to transmitter");
                            #endif

                        }

                    } else if (packetBuffer.payloadType == PAYLOAD_TYPE::PAYLOAD_ACK && _packetSendStatus == SEND_STATUS::SENDSTATUS_SENDING && _sendPacket.returnAck) {

                        #ifdef DEBUGRADIO
                            Serial.println("Received Data was an ACK");
                        #endif

                        if (packetBuffer.systemID == _activeNodePacketBuffer.destinationID) {
                            _packetSendStatus = SEND_STATUS::SENDSTATUS_SUCCESS;
                        }

                    }

                    
                } else {

                    #ifdef DEBUGRADIO
                        Serial.println("Received packet failed data checks!");
                    #endif

                }

            } 
            #ifdef DEBUGRADIO
                else {
                    Serial.println("Data was 0 bytes long! CRITICAL ERROR"); 
                    
                }
            #endif

            _sending = false;
        
        }
        
        if (irqStatus & (IRQ_RX_TIMEOUT)) {

            #ifdef DEBUGRADIO
                Serial.println("Interrupt says rx timed out!");
            #endif

            _sending = false;

        } 
        
        if (irqStatus & (IRQ_TX_DONE)) {

            #ifdef DEBUGRADIO
                Serial.println("Interrupt says tx done!");
            #endif

            _sending = false;

        }

        if (irqStatus & (IRQ_TX_TIMEOUT)) {

            #ifdef DEBUGRADIO
                Serial.println("Interrupt says tx timeout!");
            #endif

            _sending = false;

        }

        _radio.clearIrqStatus(IRQ_RADIO_ALL); 

        if (_receiving) {

            #ifdef DEBUGRADIO
                Serial.println("Putting radio back into receive mode...");
            #endif

            _radio.receive(_buffer, GENERAL_BUFFER_SIZE, 0, NO_WAIT); // put radio back into receive mode

        } else if (!_sending) {

            #ifdef DEBUGRADIO
                Serial.println("Putting radio to sleep...");
            #endif

            _radio.setSleep(CONFIGURATION_RETENTION); // make radio sleep when not receiving and transmitting is done

        }

    }


    
    if ((_packetSendStatus == SEND_STATUS::SENDSTATUS_SENDING || _sendAck) && !_sending) { 


        PacketStruct packetBuffer;
        bool send = false;


        if (_sendAck) {
            _sendAck = false;



        } else if (_sendPacket.returnAck) {

            if (millis() - _nodeResendAttemptTimestamp >= _nodeTimeout/(_nodeResendAttempts+1)) {
                _nodeResendAttemptTimestamp = millis();

                if (millis() - _nodeCommsStartTimestamp >= _nodeTimeout) {

                    #ifdef DEBUGRADIO
                        Serial.print("Non lossy packet timed out. No ACK was received from receiver... ");
                    #endif

                    _packetSendStatus == SEND_STATUS::SENDSTATUS_TIMEDOUT;
                    _nodeFailedSendCounter++;

                } else {

                    #ifdef DEBUGRADIO
                        Serial.print("Non lossy packet resend attempt... ");
                    #endif

                    packetBuffer = _activeNodePacketBuffer;
                    send = true;

                }

            }

        } else if (!_sendPacket.returnAck) { // this can run right after lossy packet timeout because it is not in a else statement from above if. resend could fail and this will run right after

            #ifdef DEBUGRADIO
                Serial.print("Sending lossy packet... ");
            #endif

            packetBuffer = _sendPacket;
            send = true;
            _packetSendStatus = SEND_STATUS::SENDSTATUS_SUCCESS;
        }


        if (send) {

            _buffer[0] = PACKET_HEADER_1;
            _buffer[1] = PACKET_HEADER_2;
            _buffer[2] = packetBuffer.payloadLength;
            _buffer[3] = packetBuffer.packets;
            _buffer[4] = packetBuffer.sequence;
            _buffer[5] = packetBuffer.payloadSequence;
            _buffer[6] = packetBuffer.systemID;
            _buffer[7] = packetBuffer.destinationID;
            _buffer[8] = packetBuffer.returnAck;
            _buffer[9] = packetBuffer.payloadType;

            for (int i = 0; i < packetBuffer.payloadLength && i < GENERAL_BUFFER_SIZE; i++) {
                _buffer[10+i] = packetBuffer.payload[i];
            }

            _buffer[packetBuffer.payloadLength+10] = packetBuffer.getCheckSum();
            _buffer[packetBuffer.payloadLength+11] = PACKET_END;

            #ifdef DEBUGRADIO
                Serial.println("Transmitting at " + String(packetBuffer.power) + "dBm...");
            #endif
            if (_sleeping) { //wake radio if sleeping
                _sleeping = false;
                _radio.wake();
            }

            _radio.transmit(_buffer, (packetBuffer.payloadLength + 12), 0, packetBuffer.power, NO_WAIT);

            _sending = true;  // set this to true so we dont send a new packet until radio is done sending

        }

        /*#ifdef DEBUGRADIO
            if (packetL == 0) {
                Serial.print("FAILED!");
            } else {
                Serial.print("Success.");
            }
            Serial.println();
            Serial.println();
        #endif*/

    }


}



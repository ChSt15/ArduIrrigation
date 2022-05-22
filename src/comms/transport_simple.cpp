#include "ArduIrrigationBase/comms/transport/transport_simple.h"



Transport_Simple::Transport_Simple(NetworkPort& networkPort, uint16_t portNumber, uint16_t destinationAddress): Task_Threading("Transport Simple", eTaskPriority_t::eTaskPriority_Middle) {

    destinationAddress_ = destinationAddress;
    port_ = portNumber;

    receiveSubr_.subscribe(networkPort.getNetworkPortTopic());
    receiveSubr_.setCallbackObject(this);
    receiveSubr_.setCallbackFunction(&Transport_Simple::receivedNetworkPacket);
    receiveSubr_.setTaskToResume(*this);

}


void Transport_Simple::sendData(uint8_t const* data, uint32_t dataLength) {

    if (dataLength == 0) return;

    uint32_t numberSegments = dataLength/MAX_FRAME_SIZE + 1;

    if (dataLength%MAX_FRAME_SIZE == 0) numberSegments--;

    DataSegment segment;
    segment.dataPacket.destinationAddress = destinationAddress_;
    segment.portNumber = port_;
    segment.transmitNumber = transmitNumber_;
    segment.segmentNumber = 0;
    segment.numberSegments = numberSegments;

    for (uint32_t i = 0; i < numberSegments; i++) {

        if (i < numberSegments - 1) memcpy(segment.dataPacket.dataFrame.getDataPointer(), data + segment.segmentNumber*MAX_FRAME_SIZE, MAX_FRAME_SIZE);
        else if (dataLength%MAX_FRAME_SIZE == 0) memcpy(segment.dataPacket.dataFrame.getDataPointer(), data + segment.segmentNumber*MAX_FRAME_SIZE, MAX_FRAME_SIZE);
        else memcpy(segment.dataPacket.dataFrame.getDataPointer(), data + segment.segmentNumber*MAX_FRAME_SIZE, dataLength%MAX_FRAME_SIZE);

        transmitBuffer_.placeBack(segment, true);

        segment.segmentNumber++;

    }

    suspendUntil(NOW());

}


uint32_t Transport_Simple::dataAvailable() {

    if (receivedData_.available() == 0) return 0;

    return receivedData_[0].getNumItems(); 

}


uint32_t Transport_Simple::getData(uint8_t* receiveBuffer) {

    uint32_t numBytes = dataAvailable();

    if (numBytes == 0) return 0;

    for (uint32_t i = 0; i < numBytes; i++) receiveBuffer[i] = receivedData_[0][i];

    receivedData_.removeBack();

    return numBytes;

}


void Transport_Simple::receivedNetworkPacket(const NetworkPacket& frame) {

    if (frame.sourceAddress != destinationAddress_) return;

    DataSegment segment;
    segment.dataPacket = frame;

    unpackDataSegment(segment);

    receiveBuffer_.placeFront(segment, true);

    suspendUntil(NOW());

}
/*

void Transport_Simple::sendDataSegment(const Transport_Simple::DataSegment& frame);


bool Transport_Simple::unpackDataSegment(Transport_Simple::DataSegment& dataSegment);


void Transport_Simple::packDataSegment(Transport_Simple::DataSegment& dataSegment);


void Transport_Simple::init();


void Transport_Simple::thread();*/
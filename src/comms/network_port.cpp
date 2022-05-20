#include "ArduIrrigationBase/comms/network/network_port.h"



void NetworkPacket::encodeFrame(DataFrame& frame) const {

    frame.setDataLength(dataFrame.getDataLength() + 4);

    frame.copyContentsFromArray((uint8_t*)&sourceAddress, 2);
    frame.copyContentsFromArray((uint8_t*)&destinationAddress, 2, 0, 2);
    frame.copyContentsFromArray(dataFrame.getDataPointer(), dataFrame.getDataLength(), 0, 4);

}


void NetworkPacket::decodeFrame(const DataFrame& frame) {

    dataFrame.setDataLength(frame.getDataLength() - 4);

    memcpy((uint8_t*)&sourceAddress, frame.getDataPointer(), 2);
    memcpy((uint8_t*)&destinationAddress, frame.getDataPointer() + 2, 2);

    memcpy(dataFrame.getDataPointer(), frame.getDataPointer() + 4, frame.getDataLength() - 4);

    //frame.copyContentsToArray((uint8_t*)&sourceAddress, 2);
    //frame.copyContentsToArray((uint8_t*)&destinationAddress, 4, 2, 0);
    //dataFrame.copyContentsFromArray(frame.getDataPointer(), dataFrame.getDataLength() + 4, 4, 0);

}


NetworkPort::NetworkPort(): Task_Threading("NetworkPort", eTaskPriority_t::eTaskPriority_Middle) {
    networkSub_.subscribe(networkPortTopic_);
}


void NetworkPort::useDatalink(DataLink& datalink) {

    datalinkReceiveSub_.unsubcribe();
    datalinkReceiveSub_.subscribe(datalink.getReceivedDataTopic());

    datalinkPub_.unsubcribe();
    datalinkPub_.subscribe(datalink.getSendDataTopic());

}


void NetworkPort::setAddress(uint16_t address) {
    address_ = address;
}


Topic<NetworkPacket>& NetworkPort::getNetworkPortTopic() {
    return networkPortTopic_;
}


void NetworkPort::receivedDatalink(const DataFrame& frame) {

    NetworkPacket packet;
    packet.decodeFrame(frame);

    if (packet.sourceAddress != address_ && (packet.destinationAddress == address_ || packet.destinationAddress == NETWORK_BROADCAST_ADDRESS)) {

        networkSub_.receiveEnable(false);

        networkPortTopic_.publish(packet);

        networkSub_.receiveEnable(true);

    }

}


void NetworkPort::sendDatalink(const NetworkPacket& frame) {

    NetworkPacket packet = frame;
    packet.sourceAddress = address_;

    DataFrame dataFrame;
    packet.encodeFrame(dataFrame);

    datalinkPub_.publish(dataFrame);

}


void NetworkPort::init() {

    datalinkReceiveSub_.setCallbackObject(this);
    datalinkReceiveSub_.setCallbackFunction(&NetworkPort::receivedDatalink);

    networkSub_.setCallbackObject(this);
    networkSub_.setCallbackFunction(&NetworkPort::sendDatalink);

}


void NetworkPort::thread() {

    suspendUntil(END_OF_TIME);

}
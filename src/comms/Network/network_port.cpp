#include "network_port.h"



void NetworkPacket::encodeFrame(DataFrame& frame) const {

    frame.setDataLength(dataFrame.getDataLength() + 4);

    frame.copyContentsFromArray((uint8_t*)&sourceAddress, 2);
    frame.copyContentsFromArray((uint8_t*)&destinationAddress, 2, 0, 2);
    frame.copyContentsFromArray(dataFrame.getDataPointer(), dataFrame.getDataLength(), 0, 4);

}


void NetworkPacket::decodeFrame(const DataFrame& frame) {

    dataFrame.setDataLength(frame.getDataLength() - 4);

    frame.copyContentsToArray((uint8_t*)&sourceAddress, 2);
    frame.copyContentsToArray((uint8_t*)&destinationAddress, 2, 0, 2);
    dataFrame.copyContentsFromArray(frame.getDataPointer(), dataFrame.getDataLength(), 0, 4);

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


Topic<NetworkPacket>& NetworkPort::getNetworkPortTopic() {
    return networkPortTopic_;
}


void NetworkPort::receivedDatalink(const DataFrame& frame) {

    NetworkPacket packet;
    packet.decodeFrame(frame);

    if (packet.sourceAddress != address_) {

        datalinkPub_.receiveEnable(false);

        networkPortTopic_.publish(packet);

        datalinkPub_.receiveEnable(true);

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
    //datalinkReceiveSub_.setCallbackFunction(receivedDatalink);

    networkSub_.setCallbackObject(this);
    //networkSub_.setCallbackFunction(sendDatalink);

}


void NetworkPort::thread() {

    suspendUntil(END_OF_TIME);

}
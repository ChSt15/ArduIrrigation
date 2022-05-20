#include "ArduIrrigationBase/comms/datalink/datalink.h"



DataLink::DataLink(PhysicalLink_Abstract& physicalLink): Task_Threading("Datalink", eTaskPriority_t::eTaskPriority_Middle), physicalLink_(physicalLink) {
}


Topic<DataFrame>& DataLink::getSendDataTopic() {return sendDataTopic_;}


Topic<DataFrame>& DataLink::getReceivedDataTopic() {return receivedDataTopic_;};


void DataLink::init() {

    sendDataFrameSub_.subscribe(sendDataTopic_);

}


void DataLink::checkNextRun() {

    if (sendDataFrameSub_.available() || physicalLink_.dataAvailable() > 0) suspendUntil(0);

}


void DataLink::thread() {

    if (physicalLink_.sendReady()) {

        //Check the state of the last sent data frame
        if (sending_) {
            sending_ = false;

            if (physicalLink_.sendCollision()) {//Failed, choose a random backoff time

                uint32_t r = (int64_t)(NOW() - 23123)*4112%(backoffCounter_*backoffCounter_) + 1;

                backoffTime_ = r*5*MILLISECONDS;

                backoffCounter_++;

            } else {

                backoffCounter_ = backoffTime_ = 0;

            }

        }

        if (sendDataFrameSub_.available() && NOW() > lastSendTimestamp_ + backoffTime_) {
            encodeDataFrame();
            sendDataFrame();
            suspendUntil(lastSendTimestamp_ + backoffTime_);
        }

    }


    if (physicalLink_.dataAvailable() > 0) {

        readDataFrame();
        decodeDataFrame();

    }

}


void DataLink::encodeDataFrame() {

    uint8_t array[MAX_FRAME_SIZE];
    uint32_t arraySize = 0;

    for (int32_t i = 0; i < sendDataFrameSub_.available(); i++) {

        DataFrame& frame = sendDataFrameSub_[i];

        if (arraySize + frame.getDataLength() + 2 <= MAX_FRAME_SIZE) {

            array[arraySize] = frame.getDataLength();
            arraySize++;

            frame.copyContentsToArray(array, frame.getDataLength(), 0, arraySize);
            arraySize += frame.getDataLength();

            sendDataFrameSub_.removeElement(&frame);
            i--;

        } else break;

    }

    array[arraySize] = 0;
    arraySize++;

    dataFrameSend = DataFrame(array, arraySize);

    sendDataFrameSub_.removeBack();

}


void DataLink::decodeDataFrame() {

    uint8_t array[MAX_FRAME_SIZE];
    uint32_t arraySize = dataFrameReceived.getDataLength();

    dataFrameReceived.copyContentsToArray(array, arraySize);

    DataFrame frame;
    for (uint32_t i = 0; i < arraySize - 1;) {

        frame = DataFrame(array, i + array[i] + 1, i + 1);
        i += array[i] + 1;

        receivedDataTopic_.publish(frame);

    }

    dataFrameReceived = DataFrame();

}


void DataLink::readDataFrame() {

    uint32_t arraySize = physicalLink_.dataAvailable();
    uint8_t array[arraySize];

    physicalLink_.receiveDataBuffer(array);

    dataFrameReceived = DataFrame(array, arraySize);

}


void DataLink::sendDataFrame() {

    lastSendTimestamp_ = NOW();
    sending_ = true;

    physicalLink_.sendDataBuffer(dataFrameSend.getDataPointer(), dataFrameSend.getDataLength());

    dataFrameSend = DataFrame();

}
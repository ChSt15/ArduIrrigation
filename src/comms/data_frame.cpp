#include "data_frame.h"



DataFrame::DataFrame() {

    dataLength = 0;

}


DataFrame::DataFrame(const uint8_t* dataToCopy, uint32_t endIndex, uint32_t startIndex, uint32_t offset) {

    uint32_t copyLength = endIndex - startIndex;

    dataLength = copyLength;

    for (uint32_t i = startIndex; i < endIndex; i++) data[i + offset - startIndex] = dataToCopy[i];

}


DataFrame::~DataFrame() {

}


void DataFrame::copyContentsFromArray(const uint8_t* dataArray, uint32_t endIndex, uint32_t startIndex, uint32_t offset) {

    for (uint32_t i = startIndex; i < endIndex; i++) data[i+offset] = dataArray[i];

}


void DataFrame::copyContentsToArray(uint8_t* dataArray, uint32_t endIndex, uint32_t startIndex, uint32_t offset) const {

    for (uint32_t i = startIndex; i < endIndex; i++) dataArray[i+offset] = data[i];

}


void DataFrame::setDataLength(uint32_t size) {
    dataLength = size;
}


uint32_t DataFrame::getDataLength() const {
    return dataLength;
}


uint8_t const* DataFrame::getDataPointer() const {
    return data;
}


DataFrame& DataFrame::operator = (const DataFrame& buffer) {

    dataLength = buffer.dataLength;

    for (uint32_t i = 0; i < buffer.dataLength; i++) data[i] = buffer.data[i];

    return *this;

}



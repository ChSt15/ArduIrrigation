#include "arrayBuffer.h"



bool ArrayBuffer::_bufferWrite(uint8_t* in, uint8_t size) {

    if ((_index + size) > _size) return false; //check if write wont fit in array

    for (int i = 0; i < size; i++) _buffer[_index + i] = in[i];

    _index += size;

    return true;

}



bool ArrayBuffer::readBuffer(uint8_t* out, uint32_t size) {

    if (size > _index) return false;

    memcpy(_buffer, out, size);

    _index -= size;

    return true;

}



float ArrayBuffer::readFloat() {

    const uint8_t size = sizeof(float);

    union {
        float out;
        uint8_t bytes[size];
    } conv;


    if (_index < size) return 0; //Check if read will go out of bounds of array


    for (int i = 0; i < size; i++) {
        conv.bytes[i] = _buffer[_index - (size-i)];
    }

    _index -= size;

    return conv.out;

}



double ArrayBuffer::readDouble() {

    const uint8_t size = sizeof(double);

    union {
        double out;
        uint8_t bytes[size];
    } conv;


    if (_index < size) return 0; //Check if read will go out of bounds of array


    for (int i = 0; i < size; i++) {
        conv.bytes[i] = _buffer[_index - (size-i)];
    }

    _index -= size;

    return conv.out;

}



bool ArrayBuffer::readBool() {

    if (_index == 0) return false; //Check if read will go out of bounds of array

    _index -= 1;

    return (bool)_buffer[_index];

}



uint8_t ArrayBuffer::readUint8_t() {

    if (_index == 0) return false; //Check if read will go out of bounds of array

    _index -= 1;

    return _buffer[_index];

}



uint16_t ArrayBuffer::readUint16_t() {

    const uint8_t size = sizeof(uint16_t);

    union {
        uint16_t out;
        uint8_t bytes[size];
    } conv;


    if (_index < size) return 0; //Check if read will go out of bounds of array


    for (int i = 0; i < size; i++) {
        conv.bytes[i] = _buffer[_index - (size-i)];
    }

    _index -= size;

    return conv.out;

}



uint32_t ArrayBuffer::readUint32_t() {

    const uint8_t size = sizeof(uint32_t);

    union {
        uint32_t out;
        uint8_t bytes[size];
    } conv;


    if (_index < size) return 0; //Check if read will go out of bounds of array


    for (int i = 0; i < size; i++) {
        conv.bytes[i] = _buffer[_index - (size-i)];
    }

    _index -= size;

    return conv.out;

}



uint64_t ArrayBuffer::readUint64_t() {

    const uint8_t size = sizeof(uint64_t);

    union {
        uint64_t out;
        uint8_t bytes[size];
    } conv;


    if (_index < size) return 0; //Check if read will go out of bounds of array


    for (int i = 0; i < size; i++) {
        conv.bytes[i] = _buffer[_index - (size-i)];
    }

    _index -= size;

    return conv.out;

}



int8_t ArrayBuffer::readInt8_t() {

    if (_index == 0) return false; //Check if read will go out of bounds of array

    _index -= 1;

    return _buffer[_index];

}



int16_t ArrayBuffer::readInt16_t() {

    const uint8_t size = sizeof(int16_t);

    union {
        int16_t out;
        uint8_t bytes[size];
    } conv;


    if (_index < size) return 0; //Check if read will go out of bounds of array


    for (int i = 0; i < size; i++) {
        conv.bytes[i] = _buffer[_index - (size-i)];
    }

    _index -= size;

    return conv.out;

}



int32_t ArrayBuffer::readInt32_t() {

    const uint8_t size = sizeof(int32_t);

    union {
        int32_t out;
        uint8_t bytes[size];
    } conv;


    if (_index < size) return 0; //Check if read will go out of bounds of array


    for (int i = 0; i < size; i++) {
        conv.bytes[i] = _buffer[_index - (size-i)];
    }

    _index -= size;

    return conv.out;

}



int64_t ArrayBuffer::readInt64_t() {

    const uint8_t size = sizeof(int64_t);

    union {
        int64_t out;
        uint8_t bytes[size];
    } conv;


    if (_index < size) return 0; //Check if read will go out of bounds of array


    for (int i = 0; i < size; i++) {
        conv.bytes[i] = _buffer[_index - (size-i)];
    }

    _index -= size;

    return conv.out;

}



bool ArrayBuffer::writeBuffer(uint8_t* in, int32_t size) {

    if (size + _index > _size) return false; //check for array bounds

    memcpy(in, _buffer + _index, size);

    _index += size;

    return true;

}
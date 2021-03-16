#ifndef _ARRAYBUFFER_H_
#define _ARRAYBUFFER_H_

#include <Arduino.h>


class ArrayBuffer {
public:

	ArrayBuffer(uint16_t maxSize) {
        _size = maxSize;
        _buffer = new uint8_t[_size];
    }


	~ArrayBuffer() {
        delete [] _buffer;
    }

	

    uint32_t getSize() {return _index;}
    uint8_t* getPtr() {return _buffer;};

    void clear() {_index = 0;}
    void setIndex(uint32_t index) {_index = min(index, _size);}

    bool readBuffer(uint8_t* out, uint32_t size);

    bool writeFloat(float in) {return _bufferWrite((uint8_t*)&in, 4);}
    bool writeDouble(double in) {return _bufferWrite((uint8_t*)&in, 8);}
    bool writeBool(bool in) {return _bufferWrite((uint8_t*)&in, 1);}
    bool writeUint8_t(uint8_t in) {return _bufferWrite((uint8_t*)&in, 1);}
    bool writeUint16_t(uint16_t in) {return _bufferWrite((uint8_t*)&in, 2);}
    bool writeUint32_t(uint32_t in) {return _bufferWrite((uint8_t*)&in, 4);}
    bool writeUint64_t(uint16_t in) {return _bufferWrite((uint8_t*)&in, 8);}
    bool writeInt8_t(int8_t in) {return _bufferWrite((uint8_t*)&in, 1);}
    bool writeInt16_t(int16_t in) {return _bufferWrite((uint8_t*)&in, 2);}
    bool writeInt32_t(int32_t in) {return _bufferWrite((uint8_t*)&in, 4);}
    bool writeInt64_t(int16_t in) {return _bufferWrite((uint8_t*)&in, 8);}


    
    bool writeBuffer(uint8_t* in, int32_t size);

    float readFloat();
    double readDouble();
    bool readBool();
    uint8_t readUint8_t();
    uint16_t readUint16_t();
    uint32_t readUint32_t();
    uint64_t readUint64_t();
    int8_t readInt8_t();
    int16_t readInt16_t();
    int32_t readInt32_t();
    int64_t readInt64_t();
	


private:

    bool _bufferWrite(uint8_t* in, uint8_t size);

	uint8_t *_buffer;

	uint32_t _size;

	uint32_t _index = 0;

};






#endif
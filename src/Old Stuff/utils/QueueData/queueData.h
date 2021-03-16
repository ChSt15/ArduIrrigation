#ifndef _QUEUEDATA_H_
#define _QUEUEDATA_H_

#include <Arduino.h>


template <typename T>
class Queue {
public:

	Queue(uint16_t _size) {
        size = _size;
        queue = new T[size];
    }


	~Queue() {
        delete [] queue;
    }

	T deQueue(){

        T obj;

        if (numElements == 0) return obj;

        obj = queue[tail];

        tail++;
        numElements--;

        if (tail >= size) tail = 0;

        return obj;

    }

	bool enQueue(T in) {

        if (numElements == size) return false;

        queue[head] = in;

        head++;
        numElements++;
        if (head == size) head = 0;

        return true;

    }

	bool isEmpty() { return numElements == 0 ? true : false; };
	bool isFull() { return numElements == size ? true : false; };
	T rear() { return queue[head == 0 ? (size - 1) : (head - 1)]; };
	T front() { return queue[tail]; };


private:

	T *queue;

	uint16_t head = 0;
	uint16_t tail = 0;

	uint16_t size;

	uint16_t numElements = 0;

};






#endif
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <Arduino.h>

#include "definitions.h"
#include "utils/dataStructs.h"
#include "comms/CommsLib.h"
#include "utils/arrayBuffer/arrayBuffer.h"

#include "system/system.hpp"


#define NETWORK_TXPOWER 20
#define BUFFER_SIZE 1000

#define NONLOSSY_REQUEST false
#define NONLOSSY_SEND false


namespace Network {

    void setup();
    void loop();

    void sleep();
    void wake();
    void shutdown();

    bool busy();

    bool requestTime(uint8_t node);
    bool requestSystemData(uint8_t node);

    bool timeSend(DateTime time, uint8_t node);
    bool systemDataSend(SystemStatus status, uint8_t node);

};



#endif
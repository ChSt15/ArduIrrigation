#ifndef NETWORK_H
#define NETWORK_H



#include "stdint.h"

#include "KraftKontrol/utils/topic.h"
#include "KraftKontrol/utils/topic_subscribers.h"

#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"

#include "../data_frame.h"
#include "../Datalink/datalink.h"



constexpr uint16_t NETWORK_BROADCAST_ADDRESS = UINT16_MAX;



class NetworkPort;


struct NetworkPacket {
friend NetworkPort;
public:

    uint16_t sourceAddress = 0;
    uint16_t destinationAddress = 0;

    DataFrame dataFrame;


private:

    void encodeFrame(DataFrame& frame) const;
    void decodeFrame(const DataFrame& frame);


};


class NetworkPort: public Task_Threading {
protected:

    ///Network IP Address
    uint16_t address_ = 0;
    
    ///Topic where send data is published.
    Topic<NetworkPacket> networkPortTopic_;

    
    Callback_Subscriber<DataFrame, NetworkPort> datalinkReceiveSub_;
    Callback_Subscriber<NetworkPacket, NetworkPort> networkSub_;

    Topic_Publisher<DataFrame> datalinkPub_;


public:

    NetworkPort();

    /**
     * @brief Uses given datalink to connect to network
     * 
     * @param datalink 
     */
    void useDatalink(DataLink& datalink);

    /**
     * @brief Set the Port Address
     * 
     * @param address 
     */
    void setAddress(uint16_t address);

    /**
     * @brief Get the Network Port Topic where all network information enters
     * 
     * @return Topic<NetworkPacket>& 
     */
    Topic<NetworkPacket>& getNetworkPortTopic();


private:

    void receivedDatalink(const DataFrame& frame);

    void sendDatalink(const NetworkPacket& frame);


    void init();

    void thread();


};


#endif
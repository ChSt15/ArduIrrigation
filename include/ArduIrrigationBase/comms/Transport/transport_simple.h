#ifndef TRANSPORT_L_H
#define TRANSPORT_L_H



#include "stdint.h"

#include "KraftKontrol/utils/topic.h"
#include "KraftKontrol/utils/topic_subscribers.h"

#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"

#include "../data_frame.h"
#include "../Network/network_port.h"



class Transport_Simple: public Task_Threading {
private:

    struct DataSegment {
    public:

        //What port this segment belongs to.
        uint16_t portNumber = 0;

        //Which segment this is. One or more segments make up a transmition.
        uint8_t segmentNumber = 0;
        //Which transmition this belongs to. One transmition is made of 1 or more segments
        uint8_t transmitNumber = 0;

        //Number of segments make up this transmit
        uint8_t numberSegments = 0;

        NetworkPacket dataPacket; 


    };


    ///Network IP Address
    uint16_t port_ = 0;

    //Destination Address
    uint16_t destinationAddress_ = 0;

    ///Current transmit number
    uint8_t transmitNumber_ = 0;

    ///Current receive number
    uint8_t receiveNumber_ = 0;
    ///Current receive segment
    uint8_t segmentNumber_ = 0;
    
    ///Buffer containing received datagrams
    Buffer<DataSegment, 50> receiveBuffer_;
    ///Buffer containing datagrams to transmitt
    Buffer<DataSegment, 50> transmitBuffer_;

    ///Buffer containing received data
    Buffer<List<uint8_t>, 10> receivedData_; 

    ///Receives new packets
    Callback_Subscriber<NetworkPacket, Transport_Simple> receiveSubr_;


public:

    /**
     * @brief Construct a new Transport_Simple object
     * 
     * @param networkPort What network to use.
     * @param portNumber Port number. Allows for multiple transport channels to one address
     * @param destinationAddress Other Address to communicate with.
     */
    Transport_Simple(NetworkPort& networkPort, uint16_t portNumber, uint16_t destinationAddress);


    /**
     * @brief Sends given data to destination address given at constructor.
     * 
     * @param data Pointer to data begin
     * @param dataLength data length in number of bytes
     */
    void sendData(uint8_t const* data, uint32_t dataLength);

    /**
     * @brief if data was received and ready, then this will return the number of bytes. If nothing available, then 0
     * 
     * @return uint32_t 
     */
    uint32_t dataAvailable();

    /**
     * @brief Writes received data into given receivebuffer pointer. Returns the number of bytes written into receive buffer.
     * 
     * @param receiveBuffer Pointer to area to receive data
     * @return uint32_t 
     */
    uint32_t getData(uint8_t* receiveBuffer);


private:

    void receivedNetworkPacket(const NetworkPacket& frame);

    void sendDataSegment(const Transport_Simple::DataSegment& frame);

    /**
     * @brief Unpacks the given data segment from packet. 
     * 
     * @param dataSegment 
     * @return true if correct
     * @return false if error
     */
    bool unpackDataSegment(Transport_Simple::DataSegment& dataSegment);

    /**
     * @brief Packs data segment into packet
     * 
     * @param dataSegment 
     */
    void packDataSegment(Transport_Simple::DataSegment& dataSegment);


    void init();

    void thread();


};


#endif
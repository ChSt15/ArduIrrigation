#ifndef PHYSICAL_LINK_ABSTRACT_H
#define PHYSICAL_LINK_ABSTRACT_H



#include "stdint.h"



class PhysicalLink_Abstract {
public:

    /**
     * @brief Max size of frame in bytes that can be sent.
     * 
     * @return uint32_t 
     */
    virtual uint32_t maxFrameSize() = 0;

    
    /**
     * @brief If the link is ready to send another data buffer.
     * 
     * @return true 
     * @return false 
     */
    virtual bool sendReady() = 0;

    /**
     * @brief Sends the given buffer.
     * 
     * @param buffer Pointer to array of data (frame) to be sent
     * @param frameSize Size of given frame in bytes
     */
    virtual void sendDataBuffer(const uint8_t* buffer, uint32_t frameSize) = 0;

    /**
     * @brief If the last send attempt failed (Collision, buffer full etc.).
     * 
     * @return true 
     * @return false 
     */
    virtual bool sendCollision() = 0;


    /**
     * @brief Gets the size of the received frame in bytes. Is 0 if no frame was received.
     * 
     * @return uint32_t 
     */
    virtual uint32_t dataAvailable() = 0;

    /**
     * @brief Places received frame into given buffer.
     * 
     * @param buffer Pointer to array to receive frame. Must be equal or greater in size returned by dataAvailable()
     */
    virtual void receiveDataBuffer(uint8_t* buffer) = 0;

};


#endif
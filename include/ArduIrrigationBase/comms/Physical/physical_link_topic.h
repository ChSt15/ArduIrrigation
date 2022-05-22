#ifndef PHYSICAL_LINK_TEST_H
#define PHYSICAL_LINK_TEST_H



#include "physical_link_abstract.h"

#include "KraftKontrol/utils/topic.h"
#include "KraftKontrol/utils/topic_subscribers.h"

#include "../data_frame.h"


#include "Arduino.h"



class PhysicalLink_Topic: public PhysicalLink_Abstract, public Task_Threading {
private:

    Simple_Subscriber<DataFrame> subr_;

    bool collision_ = false;
        
    DataFrame frameToSend_;
    DataFrame receivedFrame_;

    static bool sending_;


public:

    PhysicalLink_Topic(Topic<DataFrame>& topic): Task_Threading("PhysicalLink Test", eTaskPriority_t::eTaskPriority_Realtime, 100*MILLISECONDS) {subr_.subscribe(topic);}


    /**
     * @brief Max size of frame in bytes that can be sent.
     * 
     * @return uint32_t 
     */
    uint32_t maxFrameSize() {return MAX_FRAME_SIZE;};

    
    /**
     * @brief If the link is ready to send another data buffer.
     * 
     * @return true 
     * @return false 
     */
    bool sendReady() {return !sending_;}

    /**
     * @brief Sends the given buffer.
     * 
     * @param buffer Pointer to array of data (frame) to be sent
     * @param frameSize Size of given frame in bytes
     */
    void sendDataBuffer(const uint8_t* buffer, uint32_t frameSize) {

        if (sending_) {
            collision_ = true;
            frameToSend_ = DataFrame();
            return;
        }
        else collision_ = false;

        frameToSend_ = DataFrame(buffer, frameSize);
        sending_ = true;

    }

    /**
     * @brief If the last send attempt failed (Collision, buffer full etc.).
     * 
     * @return true 
     * @return false 
     */
    bool sendCollision() {return collision_;}


    /**
     * @brief Gets the size of the received frame in bytes. Is 0 if no frame was received.
     * 
     * @return uint32_t 
     */
    uint32_t dataAvailable() {return receivedFrame_.getDataLength();}

    /**
     * @brief Places received frame into given buffer.
     * 
     * @param buffer Pointer to array to receive frame. Must be equal or greater in size returned by dataAvailable()
     */
    void receiveDataBuffer(uint8_t* buffer) {
        receivedFrame_.copyContentsToArray(buffer, receivedFrame_.getDataLength());
        receivedFrame_ = DataFrame();
    }


    void thread() override {

        if (frameToSend_.getDataLength() > 0) {
            subr_.publish(frameToSend_);
            sending_ = false;
            frameToSend_ = DataFrame();
        }

        if (subr_.isDataNew()) {
            receivedFrame_ = subr_.getItem();
        }

    }

};


#endif
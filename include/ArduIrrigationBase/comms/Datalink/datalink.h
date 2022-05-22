#ifndef DATA_LINK_H
#define DATA_LINK_H



#include "stdint.h"

#include "KraftKontrol/utils/topic.h"
#include "KraftKontrol/utils/topic_subscribers.h"
#include "KraftKontrol/utils/system_time.h"

#include "KraftKontrol/utils/Simple-Schedule/task_threading.h"

#include "../data_frame.h"

#include "../Physical/physical_link_abstract.h"



class DataLink: public Task_Threading {
protected:
    
    ///Topic where send data is published.
    Topic<DataFrame> sendDataTopic_;
    Buffer_Subscriber<DataFrame, 10> sendDataFrameSub_;

    ///Topic where received data is published.
    Topic<DataFrame> receivedDataTopic_;   

    ///DataFrame to send data. Could contain multiple other frames
    DataFrame dataFrameSend;

    ///DataFrame of received data. Could contain multiple other frames
    DataFrame dataFrameReceived;

    ///Reference to physical link to be used
    PhysicalLink_Abstract& physicalLink_;

    ///Timestamp of last send
    int64_t lastSendTimestamp_ = 0;

    ///Current backoff time
    int64_t backoffTime_ = 0;
    uint32_t backoffCounter_ = 0;

    ///If the physical link is currently sending the data frame
    bool sending_ = false;


public:

    DataLink(PhysicalLink_Abstract& physicalLink);

    /**
     * @brief Get the Send Data Topic object
     * 
     * @return Topic<DataFrame>& 
     */
    Topic<DataFrame>& getSendDataTopic();

    /**
     * @brief Get the Received Data Topic object
     * 
     * @return Topic<DataFrame>& 
     */
    Topic<DataFrame>& getReceivedDataTopic();


private:

    /**
     * @brief encodes data frame(s) to send
     * 
     */
    void encodeDataFrame();

    /**
     * @brief decodes received data frame(s)
     * 
     */
    void decodeDataFrame();

    /**
     * @brief send the data frame
     * 
     */
    void sendDataFrame();

    /**
     * @brief reads the received data frame
     * 
     */
    void readDataFrame();


    void checkNextRun() override;

    void init() override;

    void thread() override;


};


#endif
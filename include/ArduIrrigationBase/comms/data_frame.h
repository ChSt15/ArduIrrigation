#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H



#include "stdint.h"



#define MAX_FRAME_SIZE 200


/**
 * @brief Simple dynamic array for copying data buffers with some helping functions used for transfering data
 * 
 */
class DataFrame {
private:

    uint8_t data[MAX_FRAME_SIZE];
    uint32_t dataLength = 0;


public:

    DataFrame();

    /**
     * @brief Construct a new Data Buffer object
     * 
     * @param dataToCopy Pointer to data to copy to internals
     * @param endIndex Ending index of given array (exclusive)
     * @param startIndex starting index of given array (inclusive)
     * @param offset What index to start placing data into internals
     */
    DataFrame(const uint8_t* dataToCopy, uint32_t endIndex, uint32_t startIndex = 0, uint32_t offset = 0);

    ~DataFrame();
    

    /**
     * @brief Copies internal contents into given array
     * 
     * @param dataArray Pointer to array to receive contents
     * @param endIndex Ending index of internal contents to copy (exclusive)
     * @param startIndex Starting index of internal contents to copy (inclusive)
     * @param offset What index to start placing data into given array
     */
    void copyContentsToArray(uint8_t* dataArray, uint32_t endIndex, uint32_t startIndex = 0, uint32_t offset = 0) const;

    /**
     * @brief Copie contents from array into internals
     * 
     * @param dataArray Pointer to array to be copied
     * @param endIndex Ending index of array to copy (exclusive)
     * @param startIndex Starting index of array to copy (inclusive)
     * @param offset What index to start placing data into internals
     */
    void copyContentsFromArray(const uint8_t* dataArray, uint32_t endIndex, uint32_t startIndex = 0, uint32_t offset = 0);

    /**
     * @brief Get the number of bytes.
     * 
     * @return uint32_t 
     */
    uint32_t getDataLength() const;

    /**
     * @brief set the number of bytes.
     * 
     * @param size 
     */
    void setDataLength(uint32_t size);

    /**
     * @brief Get the Data Pointer
     * 
     * @return const uint8_t* 
     */
    uint8_t const* getDataPointer() const;

    /**
     * @brief Get the Data Pointer
     * 
     * @return uint8_t* 
     */
    uint8_t* getDataPointer();

    /**
     * @brief Copies given buffers internals into this internals
     * 
     * @param buffer 
     * @return DataFrame& 
     */
    DataFrame& operator = (const DataFrame& buffer);

};


#endif
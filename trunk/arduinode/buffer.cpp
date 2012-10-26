#include "fifo.h"

#define SUCCESS 1
#define FAIL 0

struct Buffer {
    uint8_t data[RXTX_BUFFER_SIZE];
    uint8_t read;                   // zeigt auf das Feld mit dem ältesten Inhalt
    uint8_t write;                  // zeigt immer auf leeres Feld
} rxtxbuffer = {{}, 0, 0};

struct dBuffer {
    uint8_t data[DATA_BUFFER_SIZE];   // saves multi hop data
    uint8_t dataStorageLocked;          // true if the dataStorage buffer is in use. Cleared when data has passed with an ack response
    uint8_t read;                   // zeigt auf das Feld mit dem ältesten Inhalt
    uint8_t write;                  // zeigt immer auf leeres Feld
} dataBuffer = {{}, 0, 0};


uint8_t dataStorageIn(uint8_t byte) {
    if(dataBuffer.dataStorageLocked == true) 
        return FAIL;
    if(dataBuffer.write == DATA_BUFFER_SIZE) 
        return FAIL;
    else    
        dataBuffer.data[dataBuffer.write+1] = byte;
}

uint8_t dataStorageOut(uint8_t *pByte)
{
    *pByte = dataBuffer.data[rxtxbuffer.read];
    rxtxbuffer.read = (rxtxbuffer.read+1);
    return SUCCESS;
}

void dataStorageLock()
{
    dataBuffer.dataStorageLocked = true;
    dataBuffer.read  = 0;
    dataBuffer.write = 0;
}

void dataStorageUnLock()
{
    dataBuffer.dataStorageLocked = false;
}


// mhddata = multi hop data, save this into the array, not the ring buffer
uint8_t BufferIn(uint8_t byte)
{
    //   Serial.print(" #"); Serial.print(byte, DEC); 
    uint8_t next = ((rxtxbuffer.write + 1) & RXTX_BUFFER_MASK);
    if (rxtxbuffer.read == next) {
        #if BUFFERDEBUG
        Serial.println("#1");
        #endif
        return FAIL;
    }
//     rxtxbuffer.data[rxtxbuffer.write] = byte;
    rxtxbuffer.data[rxtxbuffer.write & RXTX_BUFFER_MASK] = byte; // absolut Sicher
    rxtxbuffer.write = next;
    return SUCCESS;
}

uint8_t BufferOut(uint8_t *pByte)
{
    if (rxtxbuffer.read == rxtxbuffer.write) {
        #if BUFFERDEBUG
        Serial.println("#2");
        #endif
        return FAIL;
    }
    *pByte = rxtxbuffer.data[rxtxbuffer.read];
    rxtxbuffer.read = (rxtxbuffer.read+1) & RXTX_BUFFER_MASK;
    return SUCCESS;
}

void printBuffer()
{
    Serial.println(" "); 
    Serial.print("buffer: "); 
    for(byte i=0; i<RXTX_BUFFER_SIZE; i++) {
        Serial.print(rxtxbuffer.data[i], DEC); 
        Serial.print(" "); 
    }
}

uint8_t readBufferIndex(uint8_t index)
{
    return (rxtxbuffer.data[ (rxtxbuffer.read + index) & RXTX_BUFFER_MASK ]);
}


void flushBuffer(uint8_t del)
{ 
 
    rxtxbuffer.read  = (rxtxbuffer.read   + del) & RXTX_BUFFER_MASK;
//     rxtxbuffer.write = rxtxbuffer.read;   
//     Serial.print(" del:"); 
//     Serial.print(del, DEC); 
//     Serial.print(" r:"); 
//     Serial.print(rxtxbuffer.read, DEC);    
//     Serial.print(" w:"); 
//     Serial.println(rxtxbuffer.write, DEC); 
}

#ifndef FIFO_H
#define FIFO_H

#include "WProgram.h"

#define RXTX_BUFFER_SIZE 16 // muss 2^n betragen (8, 16, 32, 64 ...)
#define RXTX_BUFFER_MASK (RXTX_BUFFER_SIZE-1) // Klammern auf keinen Fall vergessen

#define DATA_BUFFER_SIZE 16

#define RX_PACKET_LENGTH readBufferIndex(0)
#define RX_NODE_TO       readBufferIndex(1)
#define RX_NODE_FROM     readBufferIndex(2)
#define RX_PACKETTYPE    readBufferIndex(3)


extern struct Buffer buffer;

uint8_t dataStorageIn(uint8_t byte);

uint8_t dataStorageOut(uint8_t *pByte);

void dataStorageLock();

void dataStorageUnLock();


uint8_t BufferIn(uint8_t byte);

uint8_t BufferOut(uint8_t *pByte);

uint8_t readBufferIndex(uint8_t index);

void flushBuffer(uint8_t del);

void printBuffer();


#endif
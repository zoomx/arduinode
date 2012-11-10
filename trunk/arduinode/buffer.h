#ifndef FIFO_H
#define FIFO_H

#include "WProgram.h"

#define RXTX_BUFFER_SIZE 16 // muss 2^n betragen (8, 16, 32, 64 ...)
#define RXTX_BUFFER_MASK (RXTX_BUFFER_SIZE-1) // Klammern auf keinen Fall vergessen

#define DATA_BUFFER_SIZE 16

#define RX_PACKET_LENGTH 	readBufferIndex(0, RXBUFFER)
#define RX_NODE_TO                readBufferIndex(1, RXBUFFER)
#define RX_NODE_FROM		readBufferIndex(2, RXBUFFER)
#define RX_PACKETTYPE		readBufferIndex(3, RXBUFFER)
#define RX_DATABYTE_1		readBufferIndex(4, RXBUFFER)

#define TX_PACKET_LENGTH 	readBufferIndex(0, TXBUFFER)
#define TX_NODE_TO                readBufferIndex(1, TXBUFFER)
#define TX_NODE_FROM		readBufferIndex(2, TXBUFFER)
#define TX_PACKETTYPE 		readBufferIndex(3, TXBUFFER)


#define RXBUFFER   0
#define TXBUFFER   1

typedef struct {
	uint8_t fifo[RXTX_BUFFER_SIZE];
	uint8_t read; 				// points to read pos
	uint8_t write;				// points to write pos
	uint8_t newPacket;		// bool 
} buffer;
extern buffer rxBuffer, txBuffer;


uint8_t dataStorageIn(uint8_t byte);

uint8_t dataStorageOut(uint8_t *pByte);

void dataStorageLock();

void dataStorageUnLock();



uint8_t RXBufferIn(uint8_t byte) ;
uint8_t RXBufferOut(uint8_t *pByte);

uint8_t TXBufferIn(uint8_t byte);
uint8_t TXBufferOut(uint8_t *pByte);

uint8_t readBufferIndex(uint8_t index, uint8_t buffertype) ;

void flushRXBuffer(uint8_t del);

void printBuffer();


#endif

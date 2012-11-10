#include "buffer.h"

#define SUCCESS 1
#define FAIL 0



buffer rxBuffer, txBuffer;


/**
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
***/

/** RX Buffer		***/
uint8_t RXBufferIn(uint8_t byte) {
	uint8_t next = ((rxBuffer.write + 1) & RXTX_BUFFER_MASK);
	if (rxBuffer.read == next) {
		#if BUFFERDEBUG
		Serial.println("#1");
		#endif
		return FAIL;
	}
	rxBuffer.fifo[rxBuffer.write & RXTX_BUFFER_MASK] = byte; // absolut Sicher
	rxBuffer.write = next;
	return SUCCESS;
}

uint8_t RXBufferOut(uint8_t *pByte) {
	if (rxBuffer.read == rxBuffer.write) {
		#if BUFFERDEBUG
		Serial.println("#2");
		#endif
		return FAIL;
	}
	*pByte = rxBuffer.fifo[rxBuffer.read];
	rxBuffer.read = (rxBuffer.read+1) & RXTX_BUFFER_MASK;
	return SUCCESS;
}



/** TX Buffer		***/
uint8_t TXBufferIn(uint8_t byte) {
	uint8_t next = ((txBuffer.write + 1) & RXTX_BUFFER_MASK);
	if (txBuffer.read == next) {
		#if BUFFERDEBUG
		Serial.println("#1");
		#endif
		return FAIL;
	}
	txBuffer.fifo[txBuffer.write & RXTX_BUFFER_MASK] = byte; // absolut Sicher
	txBuffer.write = next;
	return SUCCESS;
}

uint8_t TXBufferOut(uint8_t *pByte) {
	if (txBuffer.read == txBuffer.write) {
		#if BUFFERDEBUG
		Serial.println("#2");
		#endif
		return FAIL;
	}
	*pByte = txBuffer.fifo[txBuffer.read];
	txBuffer.read = (txBuffer.read+1) & RXTX_BUFFER_MASK;
	return SUCCESS;
}


/**
void printBuffer()
{
	Serial.println(" "); 
	Serial.print("buffer: "); 
	for(byte i=0; i <RXTX_BUFFER_SIZE; i++) {
		Serial.print(rxtxbuffer.data[i], DEC); 
		Serial.print(" "); 
	}
}
**/

uint8_t readBufferIndex(uint8_t index, uint8_t buffertype) {
	switch(buffertype) {
		case RXBUFFER:
			return (rxBuffer.fifo[ (rxBuffer.read + index) & RXTX_BUFFER_MASK ]);
		case TXBUFFER:
			return (txBuffer.fifo[ (txBuffer.read + index) & RXTX_BUFFER_MASK ]);
	}
}


void flushRXBuffer(uint8_t del) { 
 
	rxBuffer.read  = 0;
	rxBuffer.write = 0;   
	
//	rxBuffer.read  = (rxBuffer.read + del) & RXTX_BUFFER_MASK;
//	rxBuffer.write = rxBuffer.read;   
//     Serial.print(" del:"); 
//     Serial.print(del, DEC); 
//     Serial.print(" r:"); 
//     Serial.print(rxtxbuffer.read, DEC);    
//     Serial.print(" w:"); 
//     Serial.println(rxtxbuffer.write, DEC); 
}

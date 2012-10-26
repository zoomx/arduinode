/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#include "phy.h"
#include "mac.h"
#include "config.h"
/***********************************************************************
 * TO DO:                                                              *
 * crc check, byte buffer???                                           *
 ***********************************************************************/

/************************************************************************
 * sends (b-a) bytes of data, headers are filled while sending          *
 ************************************************************************/
void phySendPacket(unsigned char to, unsigned char from, unsigned char type, unsigned char *data, unsigned char length) {
  enableIROut(38);
//   PORTB |= 1;			// led on
//  BufferIn(STARTSYMBOL);
  BufferIn(length + 4);	// add one for the pkt length info
  BufferIn(to);
  BufferIn(from);
  BufferIn(type);
  for(unsigned char i=0; i<length; i++) {
    BufferIn(data[i]);	// send i data bytes
  }
  // FIXME send buffer direct
//   closeConnection();		// enable ir rcv
    SENDBUFFER();
//   PORTB &= 0;			// led off
}

void printPkt() {
  unsigned char i = 0; // start with pkt length
  while(i < readBufferIndex(0) && i < RXTX_BUFFER_SIZE) {
    Serial.print(readBufferIndex(i), HEX);
    Serial.print(" ");
    i++;
  }
}

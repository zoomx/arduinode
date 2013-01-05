/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef packetDef.h
#define packetDef.h

#include "includes.h"



extern unsigned char MASTERNODE;

#define PHY_CHANNEL_FREE_TIME 10	// how many ms the channel has to be free to set PHY_CHANNEL_FREE = true
#define PHY_CHANNEL_FREE_RESET_TIME 500 // how many ms to reset connection state 
#define PKTTIMEOUT 500				// how many ms to wait for an ACK before resending the pkt
#define PKTRETRY 2					// how many times to retry until to delete this node from the node table
#define BROADCASTADDR 255			// BEACONS use this, this goes out to every node


/******** Packet organisation *************************************************************************
  RTS/CTS/ACK:  | length | toAddr | fromAddr |RTS/CTS/ACK|
  BEACON:         | length | toAddr | fromAddr | BEACON | HTM     |
  DATA:               | length | toAddr | fromAddr | DATA      | byte        | byte       | byte    ...
  INID:                | length | toAddr | fromAddr | INID       | nodeKey | ID(a)      | ID(b) | ID(n) |
  ANID:               | length | toAddr | fromAddr | ANID      | newID    | nodeKey | ID(a) | ID(b) | ID(n) |
  rxbuff[]                 [0]	     [1]       [2]              [3]             [4]           [5]          [6]      [7]
*******************************************************************************************************/


/******** packet type number ************/
#define RTS 	0	// Ready To Send
#define CTS  	1	// Clear To Send
#define ACK  	2	// Acknowledgement
#define PING 	3
#define ECHO 	4
#define DATA 	5   	// must contain at least 1 byte of payload
#define BEACON  7	// contains HTM as 1 data byte
#define INID	8	// I NeedID, node needs an ID
#define ANID	9	// allocated new ID, response to INID from the master node


#endif

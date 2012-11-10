/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef transport.h
#define transport.h

#include "routing.h"
#include "packetDef.h"
#include "buffer.h"

volatile extern unsigned char NEWDATA, NEWACK, NEWCTS, PINGED;
volatile extern unsigned char NEWPACKETRECEIVED, NEWRXDATA;

bool pktDaemon();
void manageNetwork();
void printPacketStatus();
#endif

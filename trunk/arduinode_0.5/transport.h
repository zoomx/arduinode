/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "includes.h"

volatile extern unsigned char NEWDATA, NEWACK, NEWCTS, PINGED;
volatile extern unsigned char NEWPACKETRECEIVED, NEWRXDATA;

uint8_t pktDaemon();
void manageNetwork();
void printPacketStatus();
#endif

/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef ARDUINODE_H
#define ARDUINODE_H

#include "driver/rxtx_driver.h"

#include "globals.h"
#include "config.h"
#include "phy.h"
#include "mac.h"
#include "routing.h"
#include "transport.h"

extern bool NEWRXDATA, NEWPACKETRECEIVED, NODE_IR_ACTIVE, PHY_CHANNEL_FREE;


/************************************************************************
 * initialization of the arduiNode, call this once in setup() 		*
 * the argument has to be MASTER or SLAVE				*
 * you should make sure to have only 1 master per net
 ************************************************************************/
void arduiNodeSetup(unsigned char NODETYPE);

/************************************************************************
 * turn the status led on or off					*
 ************************************************************************/
void statusLED(unsigned char led);


#endif

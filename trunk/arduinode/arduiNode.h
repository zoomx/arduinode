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
#include "includes.h"

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <stdlib.h>
#include "scheduler.h"
#include <../pt/pt.h>
//#include "../pt/pt.h"	// FIXME
           

#include "routing.h"
#include "driver/rxtx_driver.h"


extern uint8_t NODE_IR_ACTIVE, PHY_CHANNEL_FREE;


/************************************************************************
 * initialization of the arduiNode, call this once in setup() 		 
 * the argument has to be MASTER or SLAVE				 
 * you should make sure to have only 1 master per net
 ************************************************************************/
void arduiNodeSetup(unsigned char NODETYPE);

/************************************************************************
 * turn the status led on or off					 
 ************************************************************************/
void statusLED(unsigned char led);
void arduiNodeSetID(unsigned char id);

#endif

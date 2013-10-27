/* 
 * Copyright (c) 2013, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef arduiNode.h
#define arduiNode.h

#include "physicalLayer.h"
#include "macLayer.h"
#include "routingLayer.h"
#include "transportLayer.h"
#include "applicationLayer.h"

void arduiNodeSetup(unsigned char NODETYPE);

void led(unsigned char led);


#endif

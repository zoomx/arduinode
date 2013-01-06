/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef ROUTING_H
#define ROUTING_H

#include "includes.h"



//	update the node table and sort it smallest htm first.
void mangageNextNodes(unsigned char id, unsigned char htm) ;

// 	print the node table on console
void printNodes();

//	only used by the master node, gives back an incrementing number 
unsigned char allocateID();


#endif

/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef GLOBALS_H
#define GLOBALS_H



#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

typedef struct {
	unsigned char id;
	unsigned char htm;
}
node;
//	the node table used to manage the neighbour nodes
extern node nextNodes[];


extern unsigned char NODETYPE;




#endif

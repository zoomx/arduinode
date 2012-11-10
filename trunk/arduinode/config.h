/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */


#ifndef CONFIG_H
#define CONFIG_H

#include "globals.h"
#include "buffer.h"

#define STATUSLEDPIN 7

/**    driver    **/
#include "driver/rxtx_driver.h"


// config DEBUG level
#define RAWMODE 0
#define DEBUGMODUS 0
#define BYTEDEBUGMODUS 0
#define CONNECTDEBUGMODUS 0
#define BUFFERDEBUG 0
#define IDDEBUG 0
#define DATADEBUG 1
#define PRINTPACKETSTATS 1
	   
#define MASTER 1
#define SLAVE 0


/************************************************************************
 * USESTATICID 1 gives smaller code sizes but cancels dynamic ids       *
 * USESTATICID 0 will allocate ids automatic to all discovered nodes    *
 ************************************************************************/
#define USESTATICID 0

#if RAWMODE || DEBUGMODUS || BYTEDEBUGMODUS || CONNECTDEBUGMODUS || BUFFERDEBUG || IDDEBUG || DATADEBUG
#define SERIALCONSOLE 1
#else
#define SERIALCONSOLE 0
#endif


#endif

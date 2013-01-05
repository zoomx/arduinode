/* 
 * Copyright (c) 2013, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdint.h> 
#include <avr/interrupt.h>

#include "driver/rxtx_driver.h"
// #include "driver/rfm12/rfm12driver.h"   // FIXME 
// #include "driver/rfm12/rfm12.h"  // FIXME 

#include "transport.h"
#include "globals.h"


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



#include "buffer.h"
#include "scheduler.h"
#include "packetDef.h"
#include "config.h"
#include "phy.h"
#include "mac.h"
#include "routing.h"
#include "arduiNode.h"



#endif
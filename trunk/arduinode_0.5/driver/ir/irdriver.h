/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */
#ifndef DRIVER_H
#define DRIVER_H


#include "ir_ctrl.h"
#include "../rxtx_driver.h"


#define RXTX_DRIVER_INIT() IR_rxtx_init()

#define RXTX_TICK() 0

// nothing to do here
#define enable_IR_rx() IR_enable_IR_rx()

#define IR_INIT_PT_RCV() init_pt_rcv()

#define carrierSense() IR_carrierSense

#define IR_enableIROut(khz)


#define sendByte(data) IR_sendByte(data)

#define sendBuffer();  


#define INT_OFF() 
#define INT_ON()  




#endif

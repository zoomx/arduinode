/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */
#ifndef RFM12DRIVER_H
#define RFM12DRIVER_H

#include "rfm12.h"
#include "../rxtx_driver.h"

extern volatile unsigned long PHY_CHANNEL_BUSY_TIME;


#define RXTX_DRIVER_INIT() { rfm12_init();  NODE_IR_ACTIVE = 0;}

#define RXTX_TICK() rfm12_tick()

// nothing to do here
#define enable_IR_rx(); //FIXME why ';' ?
// nothing to do here
#define IR_INIT_PT_RCV() 

#define carrierSense() (PHY_CHANNEL_FREE = true)

#define enableIROut(khz) (NODE_IR_ACTIVE = true)


//#define sendByte(data) BufferIn(data)


#define SENDBUFFER() sendBufferRFM12()

#define INT_OFF() (detachInterrupt(0))
#define INT_ON()  (attachInterrupt(0, rfm12_poll, FALLING))

#endif

// /*
// 
// 
// void init_pt_rcv();
// 
// 
// static int proto_rcvPkt(struct pt *pt);
// 
// 
// /************************************************************************
//  * DUMMY FUNCTION                       *
//  ************************************************************************/
// void enable_IR_rx();	// FIXME I AM A DIRTY HACK..	
// 
// /************************************************************************
//  * Init of the rfm12 <driver                                            *
//  ************************************************************************/
// void rxtx_ctrl_init();
// 
// /************************************************************************
//  * checks if the medium is free,updates the boolean PHY_CHANNEL_FREE    *
//  ************************************************************************/
// void carrierSense();
// 
// /************************************************************************
//  * RC5 receive routine. Originally coded by Peter Dannegger, see        *
//  * www.mikrocontroller.net/topic/12216 for details.                     *
//  ************************************************************************/
// ISR(TIMER2_OVF_vect);
// 
// /************************************************************************
//  * Enables IR output. The khz value controls the modulation frequency   *
//  * in kilohertz. The IR output will be on pin 3 (OC2B)                  *
//  ************************************************************************/
// void enableIROut(int khz);
// 
// /************************************************************************
// * Send a single Byte with modified RC5 Protokoll						*
// * first 2 start bits are send, then the payload byte                    *
// ************************************************************************/
// void sendByte(unsigned char data);
// void sendBuffer();	// FIXME I AM A DIRTY HACK..	
// bool rf12_getData();		// FIXME I AM A DIRTY HACK..	
// **/
// #endif*/

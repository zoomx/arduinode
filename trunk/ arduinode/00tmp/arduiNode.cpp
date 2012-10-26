/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#include "arduiNode.h"

boolean NODETYPE;



// driver stuff
// static struct pt pt_rcvPkt;

// Global variables for the driver
bool NEWRXDATA, NEWPACKETRECEIVED, NODE_IR_ACTIVE, PHY_CHANNEL_FREE;

void arduiNodeSetup(boolean NODETYP) {
  NODETYPE = NODETYP;
  IR_INIT_PT_RCV();		// init proto rcv var called in isr
  init_pt_snd();		// init proto snd var...
  #if SERIALCONSOLE
  Serial.print("superNode v0.1");
  #endif
  if(NODETYPE == MASTER) {
    myself.id  = 1;
    myself.htm = 0;
    #if SERIALCONSOLE
    Serial.println(" MASTER");
    #endif
  }
  else if(NODETYPE == SLAVE){
    myself.id  = 0;
    myself.htm = 255;
    #if SERIALCONSOLE
    Serial.println(" SLAVE");
    #endif
  }
  else {
      #if SERIALCONSOLE
      Serial.println(" NDF");
      #endif
  }
  
  enable_IR_rx();
  
  pinMode(STATUSLEDPIN, OUTPUT);	// STATUS LED
  
  RXTX_DRIVER_INIT();
  
  
}


void statusLED(unsigned char led){
  digitalWrite(STATUSLEDPIN, led);
}

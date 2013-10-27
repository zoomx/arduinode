/* Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 * http://code.google.com/p/arduinode/
 * 
 * Example code to demonstrate the use of arduinode 
 * in an arduino sketch
 * 
 * THIS IS THE CODE FOR THE SLAVE NODE
 */

#include <arduiNode.h>

/** open the config.h file in an editor it is in the libraries/arduinode folder **/

void setup() {
  Serial.begin(57600);
  arduiNodeSetup(SLAVE);	// initialize the arduiNode library

  addToScheduler(printPacketStatus, 10000); // print send packet stats every 10s
  addToScheduler(manageNetwork, 3000); 		// send BEACON frame every 3s
}

unsigned char tx[5] = {
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE}; // some data to send
unsigned long t;

void loop() {

  // arduiNode logic, needs to be called constantly.
  pktDaemon();
  RXTX_TICK();
  schedule();


  if(millis() - t > 3000) { // send data every 3000ms
    if(sendData(tx, 5)) { 	// returns true if the DATA packet was ACKed
      Serial.println("data was send..");
    }
    else {
      Serial.println("Couln't send data!");
    }
    t= millis();
  }
}


/* Copyright (c) 2011, Jan Clement
* licenced under the GPL
*
* Author: Jan Clement <jan.clement@audiokits.de>
* http://code.google.com/p/arduinode/
* 
* Example code to demonstrate the use of arduinode 
* in an arduino sketch
* 
* THIS IS THE CODE FOR THE MASTER NODE
*/

#include <arduiNode.h>

/** open the config.h file in an editor it is in the libraries/arduinode folder **/

void setup() {
    Serial.begin(57600);
    arduiNodeSetup(MASTER);		// initialize the arduiNode library
    
	addToScheduler(manageNetwork, 3000); 		// send BEACON frame every 3s
}

// process new DATA
void dataProcessing() {

//#define RXBUFFER   0
//#define TXBUFFER   1

    if(NEWDATA) {		// GLOBAL variable, set to true if a new data packet was captured
        NEWDATA = false;	// reset the variable
        Serial.println("Got Data:");
        printPkt();		// this function will print the received packet including the header
        
        /* Example code, HOWTO process received data
        * the RX_BUFFER_SIZE check makes sure the array boundaries 
        * are respected.
        * data starts at readBufferIndex(4) and goes to (readBufferIndex(0)-1)
        * look at packetDef.h for a reference of packet structure 
        */
        unsigned char i = 4; // first data byte in the rx buffer
        unsigned int result=0;
        while(i < readBufferIndex(0,  0) && i < RXTX_BUFFER_SIZE) {
            result += readBufferIndex(i, 0);	// all the data bytes are added
            i++;
        }
        Serial.println(" ");
        Serial.print(" DATA:");
        Serial.println(result, DEC);
    }
}

void loop() {

	// arduiNode logic, needs to be called constantly.
	pktDaemon();
	RXTX_TICK();
 	schedule();
 	
    dataProcessing();   // do smth with the received data
}

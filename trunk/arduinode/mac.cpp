/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#include "mac.h"


static struct pt pt_relPktSend;

void init_pt_snd() {
   PT_INIT(&pt_relPktSend); 	// init proto snd var...
}


void closeConnection() {
	enable_IR_rx();		// enable rx, rf12 or ir
	SENDBUFFER();
}


void sendHeader(unsigned char length, unsigned char toAddr, unsigned char type) {

	enableIROut(38);

	TXBufferIn(length);		// pkt length info add +4 bytes for the header overhead!
	TXBufferIn(toAddr);
	TXBufferIn(myself.id);
	TXBufferIn(type);
}



node myself;

unsigned char timestamp;

unsigned char CSMA_channel_free() {
  //PHY_CHANNEL_FREE = true; // FIXME HACK
/**
  unsigned char rtr = 0; // retries
	while(rtr <= 10 && !PHY_CHANNEL_FREE) {
		delay(CSMA_TIME);
		rtr++;
		#if DEBUGMODUS
		Serial.print(" CHN BSY");
		#endif
	}
	if(!PHY_CHANNEL_FREE){	// FIXME HACK is updated in rfm12.c
		#if DEBUGMODUS
		Serial.println(" ");
		Serial.println(" CHN BLCK");
		#endif
		return 0;	
	}
	return 1; // true = channel is free
	**/
}

unsigned char pktRetry, pktACKed;
unsigned int errorCnt, pktCnt;	// FIXME count and calc packet statistics

// wrapper for macProtoSendReliablePacket()
bool sendData(unsigned char *dat, unsigned char length) {
	if(myself.id && myself.htm != 255) {	// i have an id and neigbhour nodes
		pktRetry = 0;
		pktCnt++;
		pktACKed = false;
		while(!pktACKed && pktRetry < PKTRETRY) {
			macProtoSendReliablePacket(&pt_relPktSend, dat, length);	
		}
		if(pktRetry >= PKTRETRY) { // max retries exceeded
			nextNodes[0].id = 0;	// delete this id
			mangageNextNodes(0, 255);	// update node table after deleting id 0
			errorCnt++;	// update the error count for the pkt stats
			#if DATADEBUG
			Serial.print(" delID");
			Serial.println(nextNodes[0].id, DEC);
			#endif
			#if DATADEBUG
			Serial.print(" sendERROR");
			#endif	
			return false;
		}
		else if (pktACKed){
			return true;	//data was send
		}
	}
	else
		return false;	// could not send, no id or no nodes or no ACK
}

static unsigned long tstamp;
static int macProtoSendReliablePacket(struct pt *pt, unsigned char *dat, unsigned char length) {

	PT_BEGIN(pt);
	tstamp = millis();
	// TODO check if PHY_CHANNEL_FREE in wait state
	PT_WAIT_UNTIL(pt,  (pktDaemon() || RXTX_TICK() || PHY_CHANNEL_FREE || millis() - tstamp > PKTTIMEOUT  ));	//FIXME should not block
	if(millis() - tstamp > PKTTIMEOUT){
		#if DATADEBUG
		Serial.println(" CHN BLK");
		#endif
		PT_RESTART(pt);
	}
	#if DATADEBUG
	if(pktRetry == 0) {	// only print once
		Serial.print(" SND via ID:");
		Serial.print(nextNodes[0].id, DEC);
	}
	#endif
	
	sendHeader(4, nextNodes[0].id, RTS);	// send a RTS
	closeConnection();
	
	#if DATADEBUG
	Serial.print(" <-rts");
	#endif
	
	//tstamp = millis();       //FIXME
	NEWCTS = false;
	PT_WAIT_UNTIL(pt, (pktDaemon() || RXTX_TICK() || NEWCTS || millis() - tstamp > PKTTIMEOUT) );	//FIXME should not block
	NEWCTS = false;
	if(millis() - tstamp > PKTTIMEOUT) {		// wrong answer or timeout detected
		pktRetry++;
		#if DATADEBUG
		Serial.print(" TO!rtry:");
		Serial.print(pktRetry, DEC);
		#endif
		PT_EXIT(pt);			// restart transmission          PT_RESTART(pt)

	}     
	#if DATADEBUG2
	 else {
		Serial.print(" ->cts");
	 }
	#endif
		
	 #if DATADEBUG
	Serial.print(" <-data");
	#endif
	 
	// send the actual data
	phySendPacket(nextNodes[0].id, myself.id, DATA, dat, length);
	
	//tstamp = millis();           //FIXME
	NEWACK = false; 
	PT_WAIT_UNTIL(pt, (pktDaemon() || RXTX_TICK() || NEWACK || millis() - tstamp > PKTTIMEOUT)  );	//FIXME should not block

	if(millis() - tstamp > PKTTIMEOUT){
		pktRetry++;
		#if DATADEBUG
		Serial.println(" ackTO");
		#endif
		PT_EXIT(pt);
	}
	 if(!NEWACK) {		// transmitted packet was not acked
		pktRetry++;
		#if DATADEBUG
		Serial.print(" ACK ERR:");
		Serial.print(pktRetry, DEC);
		#endif
		PT_EXIT(pt);			// restart transmission
	}
	
	NEWACK = false;
	
	#if DATADEBUG
	Serial.println(" TRNSF OK");
	#endif
	pktACKed = true;	
	PT_END(pt);
}

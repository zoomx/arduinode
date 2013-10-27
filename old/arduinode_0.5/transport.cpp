/* 
* Copyright (c) 2012, Jan Clement
* licenced under the GPL
*
* Author: Jan Clement <jan.clement@audiokits.de>
*
* This file is part of the arduiNode library
* http://code.google.com/p/arduinode/
*/

#include "transport.h"


volatile unsigned char NEWRXDATA, NEWPACKETRECEIVED, NEWDATA, PINGED, NEWACK, NEWCTS;

/****************************************************************************
* THE key function. Incoming packets are ordered by the specific           *
* packet type and the corresponding functions to process the given event    *
* are called. Handles all the protocol logic                                *
*****************************************************************************/
static unsigned long rsttime;	// packet deamon time
uint8_t pktDaemon() {
   INT_OFF();  // makro all interrupts are deactivated


    if(NEWRXDATA && ( (myself.id == 0 && RX_PACKETTYPE == ANID) || RX_NODE_TO == myself.id || RX_NODE_TO == BROADCASTADDR)) {
   // check if the received pkt is for me or broadcast or ANID for me
            NEWPACKETRECEIVED = true;
    }   
    #if RAWMODE
    else {
        Serial.print(" ->PNFM");    // pkt not for me
        printPkt();
    }
    #endif


if(NEWPACKETRECEIVED) { 	// FIXME check if pkt is valid
        NEWPACKETRECEIVED = false;
        NEWRXDATA = false;
        switch(RX_PACKETTYPE) { 
        case PING:
            #if CONNECTDEBUGMODUS
            Serial.print(" ->PING");
            #endif
            sendHeader(4, RX_NODE_FROM, ACK);	// answer with an ACK pkt
            SENDBUFFER();
            break;

        case RTS:
            #if CONNECTDEBUGMODUS
            Serial.print(" ->RTS");
            #endif
            sendHeader(4, RX_NODE_FROM, CTS);	// answer with a CTS pkt
	    	SENDBUFFER();
	    #if CONNECTDEBUGMODUS
            Serial.print(" <-CTS");
            #endif
            break;
	
        case CTS:
	    NEWCTS = true;
            #if CONNECTDEBUGMODUS
            Serial.print(" ->CTS");
            #endif
            break;
	
	case ACK:
	    NEWACK = true;
            #if CONNECTDEBUGMODUS
            Serial.print(" ->ACK");
            #endif
            break;
	
        case DATA:
            #if CONNECTDEBUGMODUS
            Serial.print(" ->DATA");
            #endif
            sendHeader(4, RX_NODE_FROM, ACK);	// answer with an ACK pkt
	    SENDBUFFER();
	    #if CONNECTDEBUGMODUS
            Serial.print(" <-ACK");
            #endif

            // if the node is not a master relay the data pkt
            // FIXME check not to send a pkt back to the node it came from to prevent loops
            if (NODETYPE == SLAVE) {
                sendHeader(RX_PACKET_LENGTH, nextNodes[0].id, DATA);	
                for(unsigned char i=4; i<RX_PACKET_LENGTH; i++) {
                    TXBufferIn(readBufferIndex(i, RXBUFFER));	// relay content of the pkt
                }
                 SENDBUFFER();
                #if CONNECTDEBUGMODUS
                Serial.print(" RLY:");
                printPkt();
                #endif
            }
            else {	// NODETYPE == MASTER
                NEWDATA = true;
                #if DEBUGMODUS
                	Serial.println("Got Data");
                //	printPkt();
                #endif
            }
        break;

        //	BEACON = packet containing the node ID and his "hops to master" as 1 byte of data
        //	nodes analyze this pkt to build the mesh network. 
        case BEACON: 
            if(!timestamp) {
                 timestamp = millis();
            }
            mangageNextNodes(RX_NODE_FROM, RX_DATABYTE_1);
            #if SHOWBEACONS
            Serial.print(" ->BC[");
            Serial.print(RX_NODE_FROM, DEC);		
            Serial.print("|");
            Serial.print(RX_DATABYTE_1, DEC);
            Serial.println("]");
			#endif
			#if PRINTNODETABLE
            printNodes();
            #endif
        break;

        //	INID = I need ID, the nodes request for a UID to identify itself in the network.
        // 	a new key in form of a timestamp is generated every time
        // 	the node receives a beacon until . It is the 8 bit value of millis()
        //	at this time. This key is used to identify the nodes befor they 
        //	have a UID allocated from the master node himself. It is send as 
        //	1 byte of data. The master node sends the key back when responding
        //	with an ANID packet.
        #if !USESTATICID	
        case INID:
            #if CONNECTDEBUGMODUS
            Serial.print(" ->INID");
            #endif
            #if BYTEDEBUGMODUS
            Serial.print(" TSrqst:");
            Serial.print(RX_DATABYTE_1,HEX);	// print out the timestamp (=key) associated with INID request
            #endif
            // readBufferIndex(RX_PACKET_LENGTH -1) is the ID of the next node in the chain back to the node which requested the new ID
            // if the master node receives a INID pkt, it responds with an ANID
		if (NODETYPE == MASTER) {
			sendHeader(RX_PACKET_LENGTH +1, 0, ANID);	// process request for a new ID
			TXBufferIn(allocateID());	// a freshly generated ID is also sent back
			for(unsigned char i=4; i<RX_PACKET_LENGTH; i++) {
			    TXBufferIn(readBufferIndex(i, RXBUFFER));	// relay content of the pkt
			}
			SENDBUFFER();
			#if CONNECTDEBUGMODUS
			Serial.print(" <-ANID");
			#endif
		}
		else {	// NODETYPE == SLAVE
			// the receiving node is not the master node, so the INID pkt has to be relayed
			sendHeader((RX_PACKET_LENGTH +1), nextNodes[0].id, INID);	
			for(unsigned char i=4; i<RX_PACKET_LENGTH; i++) {
			    TXBufferIn(readBufferIndex(i, RXBUFFER));	// relay content of the pkt
			}
			TXBufferIn(myself.id);	// attach nodes own id to the pkt
			SENDBUFFER();
			#if CONNECTDEBUGMODUS
			Serial.print(" RLY:");
			printPkt();
			#endif
		}
        break;

        // 	ANID = Allocated New ID
        //	This packet is transported back to the requesting node
        case ANID:	//FIXME should be only relevant if Node is MASTER
            #if CONNECTDEBUGMODUS
            Serial.print(" ->ANID");
            #endif
            if(myself.id) {	// id != 0 means the ANID can't be for me, so forward the pkt
                sendHeader(RX_PACKET_LENGTH -1, RX_PACKET_LENGTH -1, ANID);	// process request for a new ID
                for(unsigned char i=4; i<RX_PACKET_LENGTH-1; i++) {
                    TXBufferIn(readBufferIndex(i, RXBUFFER));	// relay content of the pkt
            }
             SENDBUFFER();
            #if DEBUGMODUS
            Serial.print(" RLY:");
            printPkt();
            #endif
            }
            else if(readBufferIndex(5, RXBUFFER) == timestamp){	// it's the right key, the ANID is for me!
                myself.id = readBufferIndex(4, RXBUFFER);		// ID allocated from master node
                #if CONNECTDEBUGMODUS
                Serial.print(" gotID:");
                Serial.print(myself.id,DEC);
                #endif
            }
            #if DEBUGMODUS
            Serial.print(" ->ANID key:");
            Serial.print(readBufferIndex(5, RXBUFFER),DEC);
            Serial.print("myKey:");
            Serial.print(timestamp,DEC);
            #endif
        break;
        #endif 	// USESTATICID
        }   // switch
        flushRXBuffer(RX_PACKET_LENGTH); 
    }
        //~ // FIXME packe das in den IR treiber
    //~ if(NODE_IR_ACTIVE) {
      	//~ closeConnection();	// enable receiving pkt again	
	//~ enable_IR_rx();
 	//~ NODE_IR_ACTIVE = false;
    //~ }
    INT_ON();
    return false;	// always return false, this is needed because this function is called in the sendData function
}

void manageNetwork() {// send beacon or INID 
	if(!PHY_CHANNEL_FREE) {
		reScheduleEvent(manageNetwork, 10);
		Serial.print(" CHNBSY");
		goto END;
	}
	if(myself.id) {	// node has a valid ID, send a beacon
		sendHeader(5, BROADCASTADDR, BEACON);
		TXBufferIn(myself.htm);
		SENDBUFFER();
		#if SHOWBEACONS
		Serial.print(" <-BC");
		Serial.print("<");
		Serial.print(myself.id,DEC);
		Serial.print("|");
		Serial.print(myself.htm,DEC);
		Serial.println("> ");
		#endif
	}
        if (NODETYPE == SLAVE) {
            if(timestamp) { // become active if other nodes are out there
            #if !USESTATICID
            static unsigned char WFR = false;
            if(myself.id == 0 && myself.htm != 255) { 	//node needs an ID, contact to other nodes exist
		    #if CONNECTDEBUGMODUS
		    Serial.print(" rqstID");
		    #endif
		    if(!WFR) {	// WFR = waiting for response
			rsttime = millis();
			//timestamp taken when receiving first BC
			WFR = true;
			// FIXME send rts first
		      
			sendHeader(5, nextNodes[0].id, INID);	// send INID Header
			TXBufferIn(timestamp);	// send UID key
			SENDBUFFER();
			#if DEBUGMODUS
			Serial.print(" INIDkey:");
			Serial.print(timestamp,DEC);
			#endif
		    }
		    if(millis() - rsttime > PKTTIMEOUT * myself.htm) {
			WFR = false;
			timestamp = 0;
			#if CONNECTDEBUGMODUS
			Serial.print(" INID RST");
			#endif
		    }
		}
            #endif
           }
        }
        #if SHOWBEACONS
        if(myself.htm == 255) {
        	Serial.println(" noNodes");
        }
        if(myself.id == 0) {
        	Serial.println(" noID");
        }
        #endif

    // FIXME packe das in den IR treiber
    //~ if(NODE_IR_ACTIVE) {
      	//~ //closeConnection();	// enable receiving pkt again	
	//~ enable_IR_rx();
 	//~ NODE_IR_ACTIVE = false;
    //~ }
    
	END:
	return;
}

void printPacketStatus() {
	Serial.print(" PKTSTATS");
            #if PRINTPACKETSTATS
            Serial.print(" pktCnt:");
            Serial.print(pktCnt);	
            Serial.print(" errCnt:");
            Serial.println(errorCnt);
            #endif
}

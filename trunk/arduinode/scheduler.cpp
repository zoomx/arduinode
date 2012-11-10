/* 
 * Copyright (c) 2012, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#include "scheduler.h"


#define SCHED_SLOT_SIZE 3

typedef struct{
	volatile unsigned long timer;
	unsigned int preload;
	void (*fname)();	// function to be called
} slots;

slots schedSlots[SCHED_SLOT_SIZE];
volatile unsigned long schedCnt;
byte schedSlot = 0;


void setupScheduler() {
	//~ cli();//stop interrupts

	//~ //set timer1 interrupt at 10Hz
	//~ TCCR1A = 0;// set entire TCCR1A register to 0
	//~ TCCR1B = 0;// same for TCCR1B
	//~ TCNT1  = 0;//initialize counter value to 0
	//~ // set compare match register for 1hz increments
	//~ OCR1A = 1562;// = (16*10^6) / (10*1024) - 1 (must be <65536)
	//~ // turn on CTC mode
	//~ TCCR1B |= (1 << WGM12);
	//~ // Set CS10 and CS12 bits for 1024 prescaler
	//~ TCCR1B |= (1 << CS12) | (1 << CS10);  
	//~ // enable timer compare interrupt
	//~ TIMSK1 |= (1 << OCIE1A);

	//~ sei();//allow interrupts
}


void addToScheduler(void (*f)(), int dsIntervall) {
	if(schedSlot == SCHED_SLOT_SIZE) {
		return;
	}
	schedSlots[schedSlot].preload = dsIntervall;
	schedSlots[schedSlot].fname = f;
	schedSlots[schedSlot].timer = schedCnt + schedSlots[schedSlot].preload;
	schedSlot++;
}
unsigned long scheduleT;
//ISR(TIMER1_COMPA_vect){
void schedule() {
	schedCnt = millis();
	for(byte i=0; i<SCHED_SLOT_SIZE; i++) {
		if(schedCnt == schedSlots[i].timer) {
			schedSlots[i].timer = schedCnt + schedSlots[i].preload;
			schedSlots[i].fname();
		}
	}
}

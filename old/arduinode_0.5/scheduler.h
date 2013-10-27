/* 
 * Copyright (c) 2012, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H
   
#include "includes.h"

//void setupScheduler();


void addToScheduler(void (*f)(), uint16_t msIntervall);

void schedule();

void reScheduleEvent(void (*f)(), uint16_t msIntervall);
#endif

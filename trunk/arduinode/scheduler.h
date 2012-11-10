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

#include "globals.h"
#include "config.h"

void setupScheduler();


void addToScheduler(void (*f)(), int msIntervall);

void schedule();
#endif

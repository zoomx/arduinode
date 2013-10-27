/*
 * Copyright (c) 2013, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */


#include "genericDriver.h"

void *txFunctionToUse(uint8_t);

void registerTransmitFunction(void (*txFunction)) {
	txFunctionToUse = txFunction;
}


void txSymbol(uint8_t symbol) {
	txFunctionToUse(symbol);
}

void initTXSequence() {

}

uint8_t rxSymbol() {

}

#ifndef RF12_H
#define RF12_H

// arduino core 
#include "WProgram.h"

// global driver variables
#include "../rxtx_driver.h"



void rfm12_poll();

bool rfm12_tick();

void rfm12_sendData(uint8_t *data, uint8_t length);

void rfm12_init();

void rfm12_data(uint16_t d);

uint16_t rfm12_read(uint16_t c);

uint8_t rfm12_read_int_flags_inline();

uint8_t rfm12_read_fifo_inline();

void rfm12_data_inline(uint8_t cmd, uint8_t d);

void printStatus();

void sendBufferRFM12(void);

#endif
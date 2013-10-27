#ifndef RXTX_DRIVER_H
#define RXTX_DRIVER_H

#include "../includes.h"


// choose the driver to use
#define RFM12_DRIVER
// #define IR_DRIVER


#ifdef  RFM12_DRIVER
    #include "rfm12/rfm12driver.h"
#endif

#ifdef IR_DRIVER
    #include "ir/irdriver.h"
#endif


volatile extern unsigned char NEWPACKETRECEIVED; 
extern uint8_t NODE_IR_ACTIVE, PHY_CHANNEL_FREE;






#endif

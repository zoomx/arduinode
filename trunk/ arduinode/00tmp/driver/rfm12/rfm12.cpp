#include "WProgram.h"                                                                
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "rfm12.h" 
#include "rfm12_hw.h"
#include "../../arduiNode.h"

#if 1
/** State machine **/
#define RFM12_STATUS_RGIT   0x8000
#define RFM12_STATUS_FFIT   0x8000

#define RFM12_STATUS_RSSI   0x0100
#define CHANNEL_FREE_TIME 200

/** FIFO and Reset Mode Command **/
#define RFM12_CMD_FIFORESET 0xCA00
#define RFM12_FIFORESET_SP 0x08
#define RFM12_FIFORESET_AL 0x04
#define RFM12_FIFORESET_FF 0x02
#define RFM12_FIFORESET_DR 0x01
//default fiforeset register value to clear fifo
#define CLEAR_FIFO (RFM12_CMD_FIFORESET | RFM12_FIFORESET_DR | (8<<4))
#define CLEAR_FIFO_INLINE (RFM12_FIFORESET_DR | (8<<4))
//default fiforeset register value to accept data
#define ACCEPT_DATA (RFM12_CMD_FIFORESET | RFM12_FIFORESET_DR | (8<<4) | RFM12_FIFORESET_FF)
#define ACCEPT_DATA_INLINE (RFM12_FIFORESET_DR | (8<<4) | RFM12_FIFORESET_FF)

//these are the states for the receive/transmit state machine
#define STATE_RX_IDLE 0
#define STATE_RX_ACTIVE 1
#define STATE_TX 2

// Data Rate Command
#ifndef PWRMGT_DEFAULT
#define PWRMGT_DEFAULT (RFM12_PWRMGT_DC | PWRMGMT_WKUP | PWRMGMT_LOW_BATT)
#endif
#define PWRMGMT_WKUP 0      // dont use wake up timer
#define PWRMGMT_LOW_BATT 0  // dont use low batt detector
//the receiver is turned on by default in normal mode
#define PWRMGT_RECEIVE (RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ER)

#define RFM12_INT_ON()      (attachInterrupt(0, rfm12_poll, FALLING))
#define RFM12_INT_OFF()     (detachInterrupt(0))
#define RFM12_INT_SETUP()   (attachInterrupt(0, rfm12_poll, FALLING))

//the interrupt flag register
#define RFM12_INT_FLAG EIFR // Atmega168
//the interrupt bit in the flag register
#define RFM12_FLAG_BIT (INTF0)

//default synchronization pattern
#define SYNC_MSB 0x2D
#define SYNC_LSB 0xD4

#endif

#define RFM12_UART_DEBUG   23

//! This controls the library internal state machine.
volatile uint8_t rfm12_state;

//! Number of bytes to transmit or receive.
/** This refers to the overall data size, including header data and sync bytes. */
uint8_t num_bytes, RX_COMPLETE;

//! Counter for the bytes we are transmitting or receiving at the moment.
uint8_t bytecount;


//! Transmit buffer status.
volatile uint8_t txstate;

void printStatus()
{
    Serial.print("!");
    Serial.print(rfm12_read_int_flags_inline(), BIN);
    Serial.print("#");
}

uint8_t dummyByte;
volatile uint8_t synchronization_bytes;
void rfm12_poll()
{
    RFM12_INT_OFF();
    static uint8_t status;

    static uint8_t checksum; //static local variables produce smaller code size than globals

    //first we read the first byte of the status register
    //to get the interrupt flags
    status = rfm12_read_int_flags_inline();

    //check if the fifo interrupt occurred
    if(!(status & (RFM12_STATUS_FFIT>>8))) {
        goto END;
    }
    
    //see what we have to do (start rx, rx or tx)
        switch(rfm12_state)
        {           
            case STATE_RX_IDLE:
                //read the length byte
                //remember, the first byte is the length byte           
                num_bytes = rfm12_read_fifo_inline();
                bytecount = 1;  // the first byte was already read
                
                // FIXME check if buffer free
                BufferIn(num_bytes);    // put it to the fifo

                #if RFM12_UART_DEBUG >= 23
                Serial.print(" I");
                Serial.print(num_bytes, DEC);
                #endif

                rfm12_state = STATE_RX_ACTIVE;
                
                goto END;
                /* if we're here, the buffer is full, so we ignore this transmission by resetting the fifo (at the end of the function)  */
                break;
                
            case STATE_RX_ACTIVE:
                //check if transmission is complete
                if(bytecount < num_bytes)   // FIXME ceck available length
                {
                    uint8_t data;
                    
                    //read a byte
                    data = rfm12_read_fifo_inline();
                    
                    BufferIn(data); // put it to the fifo
                    
                    #if RFM12_UART_DEBUG >= 23
                    Serial.print(" R");
                    Serial.print(data, DEC);
                    #endif
      
                    bytecount++;
                    
                    //end the interrupt without resetting the fifo
                    goto END;
                }
                
                /* if we're here, receiving is done */
                /* the fifo will be reset at the end of the function */
                
                //debug
                #if RFM12_UART_DEBUG >= 23
                Serial.println('D');
                #endif
                
                // FIXME check if bufferLocked is true before switching to STATE_RX_ACTIVE mode
                
                NEWRXDATA = true;
                //process the new packet
                pktDaemon();
                
                
                break;
                
                case STATE_TX:
                    uint8_t data;
                    // Send the 2 sync bytes before every packet
                    if(synchronization_bytes == 0)
                    {
                        bytecount = 0;  // reset the buffer counter
                        rfm12_data_inline( (RFM12_CMD_TX>>8), SYNC_MSB);
                        synchronization_bytes++;
                        goto END;
                    }
                    else if (synchronization_bytes == 1)
                    {
                        rfm12_data_inline( (RFM12_CMD_TX>>8), SYNC_LSB);
                        synchronization_bytes++;
                        num_bytes = readBufferIndex(0);

                        goto END;
                    }                  
                    if(bytecount < num_bytes) // 0 = length
                    {
                        // Now send Data bytes
                        BufferOut(&data);      
                        rfm12_data_inline( (RFM12_CMD_TX>>8), data );
                        bytecount++;
                        //debug
                        #if RFM12_UART_DEBUG >= 23
                        Serial.print(" T");
                        Serial.print(data, DEC);
                        #endif
                        //Serial.println("bytes");end the interrupt without resetting the fifo
                        goto END;
                   }
                   if(dummyByte == 0) { // send 1 byte more to avoid data coruption of the last byte
                       rfm12_data_inline( (RFM12_CMD_TX>>8), dummyByte );
                        dummyByte = 1;
                        goto END;
                   }
                    /* if we're here, we're finished transmitting the bytes */
                    /* the fifo will be reset at the end of the function */
                    
                   
                    #if RFM12_UART_DEBUG >= 23
                    Serial.println('D');
                    #endif
      
                    //turn off the transmitter and enable receiver
                    //the receiver is not enabled in transmit only mode
                    //if the wakeup timer is used, this will re-enable the wakeup timer bit
                    //the magic is done via defines
                    rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_RECEIVE);

                    //load a dummy byte to clear int status
                    rfm12_data_inline( (RFM12_CMD_TX>>8), 0xaa);
                    break;          
        }
        
        //set the state machine to idle
        rfm12_state = STATE_RX_IDLE;

        //reset the receiver fifo
        rfm12_data_inline(RFM12_CMD_FIFORESET>>8, CLEAR_FIFO_INLINE);
        rfm12_data_inline(RFM12_CMD_FIFORESET>>8, ACCEPT_DATA_INLINE);
        
        END: 
        
        RFM12_INT_ON(); //turn the int back on
}
//! Indicates that the buffer is free
#define STATUS_FREE 0
//! Indicates that the buffer is in use by the library
#define STATUS_OCCUPIED 1
//! Indicates that a receive buffer holds a complete transmission
#define STATUS_COMPLETE 2

// use it to send packets
bool rfm12_tick(void)
{   
    //collision detection is enabled by default
    #if !(RFM12_NOCOLLISIONDETECTION)
    uint16_t status;
    
    //start with a channel free count of 16, this is necessary for the ASK receive feature to work
    static uint8_t channel_free_count = 16; //static local variables produce smaller code size than globals
    #endif
    
    //debug
    #if RFM12_UART_DEBUG > 23
    static uint8_t oldstate = 23;
    uint8_t state = rfm12_state;
    if (oldstate != state)
    {
        Serial.print ("mode change: ");
        switch (state)
        {
            case STATE_RX_IDLE:
                Serial.print ('i');
                break;
            case STATE_RX_ACTIVE:
                Serial.print ('r');
                break;
            case STATE_TX:
                Serial.print ('t');
                break;
            default:
                Serial.print ('?');
        }
        Serial.println (" ");
        oldstate = state;
    }
    #endif
    
    //don't disturb RFM12 if transmitting or receiving
    if(rfm12_state != STATE_RX_IDLE)
    {
        return false;
    }   
    
    //collision detection is enabled by default
    #if !(RFM12_NOCOLLISIONDETECTION)
    //disable the interrupt (as we're working directly with the transceiver now)
    //hint: we could be losing an interrupt here 
    //solutions: check status flag if int is set, launch int and exit ... OR implement packet retransmission
    RFM12_INT_OFF();    
    status = rfm12_read(RFM12_CMD_STATUS);
    RFM12_INT_ON();
    
    //check if we see a carrier
    if(status & RFM12_STATUS_RSSI)
    {
        //yes: reset free counter and return
        channel_free_count = CHANNEL_FREE_TIME;
//         PHY_CHANNEL_FREE = false;// FIXME HACK
        return false;
    }
    
    //no: decrement counter
    channel_free_count--;
    
    //is the channel free long enough ?
    if(channel_free_count != 0)
    {
        return false;
    }
    
    //if we are here the channel is believed to be free
    PHY_CHANNEL_FREE = true; // FIXME HACK
    
    //reset the channel free count for the next decrement (during the next call..)
    channel_free_count = 1;
    #endif  
//     Serial.print ('§');
    //do we have something to transmit?
    if(txstate == STATUS_OCCUPIED)
    { //yes: start transmitting
//     Serial.print ('*');
    //disable the interrupt (as we're working directly with the transceiver now)
    //hint: we could be losing an interrupt here, too
    //we could also disturb an ongoing reception,
    //if it just started some cpu cycles ago 
    //(as the check for this case is some lines (cpu cycles) above)
    //anyhow, we MUST transmit at some point...
    
    txstate = 0;   // FIXME HACK
    RFM12_INT_OFF();
    
    //disable receiver - if you don't do this, tx packets will get lost
    //as the fifo seems to be in use by the receiver
    rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT);
    
    //set mode for interrupt handler
    rfm12_state = STATE_TX;
    // reset the sync byte mechanism
    synchronization_bytes = 0;
    dummyByte = 0;
    
    //fill 2byte 0xAA preamble into data register
    //the preamble helps the receivers AFC circuit to lock onto the exact frequency
    //(hint: the tx FIFO [if el is enabled] is two staged, so we can safely write 2 bytes before starting)
    rfm12_data(RFM12_CMD_TX | 0xAA);
    rfm12_data(RFM12_CMD_TX | 0xAA);

    //set ET in power register to enable transmission (hint: TX starts now)
    rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ET);
    
    //enable the interrupt to continue the transmission
    RFM12_INT_ON();
    }
    rfm12_poll();// FIXME
    return false;
}
/************************
* PIN DEFINITIONS
*/

#if 1

//Pin that the RFM12's slave select is connected to
#define DDR_SS DDRB
#define PORT_SS PORTB
#define BIT_SS 2

//SPI port
#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define PIN_SPI PINB
#define BIT_MOSI 3
#define BIT_MISO 4
#define BIT_SCK  5
#define BIT_SPI_SS 2
//this is the hardware SS pin of the AVR - it 
//needs to be set to output for the spi-interface to work 
//correctly, independently of the CS pin used for the RFM12

//hardware spi helper macros
#define SS_ASSERT() PORT_SS &= ~(1<<BIT_SS)
#define SS_RELEASE() PORT_SS |= (1<<BIT_SS)

#endif
//non-inlined version of rfm12_data
//warning: without the attribute, gcc will inline this even if -Os is set
void __attribute__ ((noinline)) rfm12_data(uint16_t d)
{
    SS_ASSERT();
    #if !(RFM12_SPI_SOFTWARE)
    SPDR = d>>8;
    while(!(SPSR & (1<<SPIF)));
    
    SPDR = d & 0xff;
    while(!(SPSR & (1<<SPIF)));
    
    #else
    spi_data(d >> 8   );
    spi_data(d &  0xff);
    #endif
    SS_RELEASE();
}
//non-inlined version of rfm12_read
//warning: without the attribute, gcc will inline this even if -Os is set
uint16_t __attribute__ ((noinline)) rfm12_read(uint16_t c)
{
    uint16_t retval;
    SS_ASSERT();
    
    #if !(RFM12_SPI_SOFTWARE)
    SPDR = c>>8;
    while(!(SPSR & (1<<SPIF)));
    retval = SPDR<<8;
    SPDR = c & 0xff;
    while(!(SPSR & (1<<SPIF)));
    retval |= SPDR;
    
    #else
    retval =  spi_data(c >> 8   );
    retval <<= 8;
    retval |= spi_data(c &  0xff);
    #endif
    SS_RELEASE();
    return retval;
}

/* @description reads the upper 8 bits of the status
* register (the interrupt flags)
*/
uint8_t rfm12_read_int_flags_inline()
{
    SS_ASSERT();
    #if !(RFM12_SPI_SOFTWARE)
    SPDR = 0;
    while(!(SPSR & (1<<SPIF)));
    SS_RELEASE();
    return SPDR;
    
    #else
    unsigned char x, d=d;
    PORT_MOSI &= ~(1<<BIT_MOSI);    
    for(x=0;x<8;x++){
        PORT_SCK |= (1<<BIT_SCK);
        d<<=1;
        if(PIN_MISO & (1<<BIT_MISO)){
            d|=1;
        }
        PORT_SCK &= ~(1<<BIT_SCK);
    }
    SS_RELEASE();
    return d;
    #endif
}
/* @description inline function for reading the fifo
 */
uint8_t rfm12_read_fifo_inline()
{
    SS_ASSERT();

    #if !(RFM12_SPI_SOFTWARE)
    SPDR =  ( RFM12_CMD_READ >> 8 );
    while(!(SPSR & (1<<SPIF)));

    SPDR = 0;
    while(!(SPSR & (1<<SPIF)));

    SS_RELEASE();
    return SPDR;
    
    #else
    uint8_t retval;
    spi_data( RFM12_CMD_READ >> 8 );
    retval = spi_data( 0   );

    SS_RELEASE();
    return retval;
    #endif
}


void spi_init()
{
    DDR_SPI      |= (_BV(BIT_MOSI));
    DDR_SPI      |= (_BV(BIT_SCK));
    DDR_SPI      |= (_BV(BIT_SPI_SS));
    DDR_SPI      &= ~(_BV(BIT_MISO));
    
    #if !(RFM12_SPI_SOFTWARE)
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);//SPI Master, clk/16
    #endif
}

void rfm12_data_inline(uint8_t cmd, uint8_t d)
{
    SS_ASSERT();
    #if !(RFM12_SPI_SOFTWARE)
    SPDR = cmd;
    while(!(SPSR & (1<<SPIF)));
    
    SPDR = d;
    while(!(SPSR & (1<<SPIF)));
    
    #else
    spi_data( cmd );
    spi_data( d   );
    #endif
    SS_RELEASE();
}
#if 1
//baseband of the module (either RFM12_BAND_433, RFM12_BAND_868 or RFM12_BAND_912)
#define RFM12_BASEBAND RFM12_BAND_433
//center frequency to use (+-125kHz FSK frequency shift)
#define FREQ 433175000UL
//use this for datarates >= 2700 Baud
#define DATARATE_VALUE RFM12_DATARATE_CALC_HIGH(9600.0)

//baseband selection
#if (RFM12_BASEBAND) == RFM12_BAND_433
#define RFM12_FREQUENCY_CALC(x) RFM12_FREQUENCY_CALC_433(x)
#elif (RFM12_BASEBAND) == RFM12_BAND_868
#define RFM12_FREQUENCY_CALC(x) RFM12_FREQUENCY_CALC_868(x)
#elif (RFM12_BASEBAND) == RFM12_BAND_915
#define RFM12_FREQUENCY_CALC(x) RFM12_FREQUENCY_CALC_915(x)
#else
#error "Unsupported RFM12 baseband selected."
#endif
#define RFM12_FREQUENCY_CALC_433(f) (((f)-430000000UL)/2500UL)
#define RFM12_FREQUENCY_CALC_868(f) (((f)-860000000UL)/5000UL)
#define RFM12_FREQUENCY_CALC_915(f) (((f)-900000000UL)/7500UL)

#define RFM12_CMD_DATARATE 0xC600
#define RFM12_DATARATE_CS 0x80
//calculate setting for datarates >= 2700 Baud
#define RFM12_DATARATE_CALC_HIGH(d) ((uint8_t)((10000000.0/29.0/d)-0.5))
//calculate setting for datarates < 2700 Baud
#define RFM12_DATARATE_CALC_LOW(d) (RFM12_DATARATE_CS|(uint8_t)((10000000.0/29.0/8.0/d)-0.5))




#define RFM12_CMD_FREQUENCY 0xA000

//default preamble (altrernating 1s and 0s)
#define PREAMBLE 0xAA

//default synchronization pattern
#define SYNC_MSB 0x2D
#define SYNC_LSB 0xD4

#endif
void rfm12_init(void)
{
    //initialize spi
    SS_RELEASE();
    DDR_SS |= (1<<BIT_SS);  
    spi_init();
    
    //enable internal data register and fifo
    //setup selected band
    rfm12_data(RFM12_CMD_CFG | RFM12_CFG_EL | RFM12_CFG_EF | RFM12_BASEBAND | RFM12_XTAL_12PF);
    
    //set power default state (usually disable clock output)
    //do not write the power register two times in a short time
    //as it seems to need some recovery
    rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT);
    
    //set frequency
    rfm12_data(RFM12_CMD_FREQUENCY | RFM12_FREQUENCY_CALC(FREQ) );
    
    //set data rate
    rfm12_data(RFM12_CMD_DATARATE | DATARATE_VALUE );
    
    //set rx parameters: int-in/vdi-out pin is vdi-out,
    //Bandwith, LNA, RSSI
    rfm12_data(RFM12_CMD_RXCTRL | RFM12_RXCTRL_P16_VDI 
    | RFM12_RXCTRL_VDI_FAST | RFM12_RXCTRL_BW_400 | RFM12_RXCTRL_LNA_6 
    | RFM12_RXCTRL_RSSI_79 );   
    
    //automatic clock lock control(AL), digital Filter(!S),
    //Data quality detector value 3, slow clock recovery lock
    rfm12_data(RFM12_CMD_DATAFILTER | RFM12_DATAFILTER_AL | 3);
    
    //2 Byte Sync Pattern, Start fifo fill when sychron pattern received,
    //disable sensitive reset, Fifo filled interrupt at 8 bits
    rfm12_data(RFM12_CMD_FIFORESET | RFM12_FIFORESET_DR | (8<<4));
    
    //set AFC to automatic, (+4 or -3)*2.5kHz Limit, fine mode, active and enabled
    rfm12_data(RFM12_CMD_AFC | RFM12_AFC_AUTO_KEEP | RFM12_AFC_LIMIT_4
    | RFM12_AFC_FI | RFM12_AFC_OE | RFM12_AFC_EN);
    
    //set TX Power to -0dB, frequency shift = +-125kHz
    rfm12_data(RFM12_CMD_TXCONF | RFM12_TXCONF_POWER_0 | RFM12_TXCONF_FS_CALC(125000) );
    
    //disable low dutycycle mode
    rfm12_data(RFM12_CMD_DUTYCYCLE);
    
    //disable wakeup timer
    rfm12_data(RFM12_CMD_WAKEUP);
    
    //store the syncronization pattern to the transmission buffer
    //the sync pattern is used by the receiver to distinguish noise from real transmissions
    //the sync pattern is hardcoded into the receiver
//     rf_tx_buffer.sync[0] = SYNC_MSB; //FIXME HACK
//         rf_tx_buffer.sync[1] = SYNC_LSB; //FIXME HACK
    
    //if receive mode is not disabled (default)
    #if !(RFM12_TRANSMIT_ONLY)
    //init buffer pointers
//     ctrl.rf_buffer_out = &rf_rx_buffers[0];
//     ctrl.rf_buffer_in  = &rf_rx_buffers[0];
    //ctrl.buffer_in_num = 0;
    //ctrl.buffer_out_num = 0;
    #endif /* !(RFM12_TRANSMIT_ONLY) */
    
    //low battery detector feature initialization
    #if RFM12_LOW_BATT_DETECTOR
    ctrl.low_batt = RFM12_BATT_OKAY;
    #endif /* RFM12_LOW_BATT_DETECTOR */
    
    //enable rf receiver chain, if receiving is not disabled (default)
    //the magic is done via defines
    rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_RECEIVE);
    
    //wakeup timer feature setup
    #if RFM12_USE_WAKEUP_TIMER
    //set power management shadow register to receiver chain enabled or disabled
    //the define correctly handles the transmit only mode
    ctrl.pwrmgt_shadow = (RFM12_CMD_PWRMGT | PWRMGT_RECEIVE);
    #endif /* RFM12_USE_WAKEUP_TIMER */
    
    //ASK receive mode feature initialization
    #if RFM12_RECEIVE_ASK
    adc_init();
    #endif
    
    //setup interrupt for falling edge trigger
    RFM12_INT_SETUP();
    
    //clear int flag
    rfm12_read(RFM12_CMD_STATUS);
    RFM12_INT_FLAG |= (1<<RFM12_FLAG_BIT);      
    
    //init receiver fifo, we now begin receiving.
    rfm12_data(CLEAR_FIFO);
    rfm12_data(ACCEPT_DATA);
    
    //activate the interrupt
    RFM12_INT_ON(); 
}

void sendBufferRFM12()
{    
    txstate = STATUS_OCCUPIED;
    Serial.print("rf12");
}  
    
    

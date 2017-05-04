#include <stdio.h>
#include <util/delay.h>
#include "jtag.h"
#include "avriotools.h"

/* It is supposed to work at 1Mbps */
#define CLK_DELAY() _delay_us(2)

void jtag_init(void) {
    pin_mode(&JTAG_PORT, JTAG_TCK, PM_OUTPUT);
    pin_mode(&JTAG_PORT, JTAG_TMS, PM_OUTPUT);
    pin_mode(&JTAG_PORT, JTAG_TDI, PM_OUTPUT);
    pin_mode(&JTAG_PORT, JTAG_TDO, PM_INPUT);
    pin_mode(&JTAG_PORT, JTAG_TRST, PM_OUTPUT);
    pin_lo(JTAG_PORT, JTAG_TCK);
    pin_lo(JTAG_PORT, JTAG_TMS);
    pin_lo(JTAG_PORT, JTAG_TDI);
    pin_hi(JTAG_PORT, JTAG_TRST);
}

#define _TDI_TMS_TCK(tdi, tms, tck)		\
    ((tms? _BV(JTAG_TMS): 0) |			\
     (tdi? _BV(JTAG_TDI): 0) |			\
     (tck? _BV(JTAG_TCK): 0))
#define _SET_PINS(pv, tdi, tms, tck)		\
    do {					\
	pv = JTAG_PORT;				\
	pv &= ~_TDI_TMS_TCK(1, 1, 1);		\
	pv |= _TDI_TMS_TCK(tdi, tms, tck);	\
	JTAG_PORT = pv;				\
    } while(0)
#define SEND_BIT(pv, tdi, tms)			\
    do {					\
	_SET_PINS(pv, tdi, tms, 0);		\
	CLK_DELAY();				\
	_SET_PINS(pv, tdi, tms, 1);		\
	CLK_DELAY();				\
    } while(0)
#define GET_TDO() (JTAG_PIN & _BV(JTAG_TDO))
#define SEND_GET_BIT(pv, tdi, tms, out)		\
    do {					\
	_SET_PINS(pv, tdi, tms, 0);		\
	CLK_DELAY();				\
	out = GET_TDO();			\
	_SET_PINS(pv, tdi, tms, 1);		\
	CLK_DELAY();				\
    } while(0)

void jtag_trst(void) {
    unsigned char pv;
    
    pin_lo(JTAG_PORT, JTAG_TCK);
    pin_lo(JTAG_PORT, JTAG_TRST);
    
    SEND_BIT(pv, 1, 1);
    SEND_BIT(pv, 1, 1);
    SEND_BIT(pv, 1, 1);
    SEND_BIT(pv, 1, 1);
    SEND_BIT(pv, 1, 1);
    
    pin_lo(JTAG_PORT, JTAG_TCK);
    pin_hi(JTAG_PORT, JTAG_TRST);
    
    SEND_BIT(pv, 1, 1);
}

/*!
 * Before shifting registers, TAP controller must move to last state before
 * shift state.  The state machine transite to shift state when shifting
 * starts.
 */
void jtag_tms(unsigned char *buf, int nbits) {
    int i;
    int nbytes;
    int byteoff, bitoff;
    unsigned char byte;
    unsigned char bit;
    unsigned char pv;
    
    nbytes = nbits / 8;
    for(i = 0; i < nbytes; i++) {
	byte = buf[i];
	
	bit = byte & 0x01;
	SEND_BIT(pv, 1, bit);
	
	bit = byte & 0x02;
	SEND_BIT(pv, 1, bit);
	
	bit = byte & 0x04;
	SEND_BIT(pv, 1, bit);
	
	bit = byte & 0x08;
	SEND_BIT(pv, 1, bit);
	
	bit = byte & 0x10;
	SEND_BIT(pv, 1, bit);
	
	bit = byte & 0x20;
	SEND_BIT(pv, 1, bit);
	
	bit = byte & 0x40;
	SEND_BIT(pv, 1, bit);
	
	bit = byte & 0x80;
	SEND_BIT(pv, 1, bit);
    }
    
    byte = buf[i];
    nbits %= 8;
    for(i = 0; i < nbits; i++) {
	bit = byte & (1 << i);
	SEND_BIT(pv, 1, bit);
    }
}

void jtag_shift(unsigned char *buf, int nbits) {
    int i;
    int nbits_1;
    int nbytes;
    int remain;
    int byteoff, bitoff;
    unsigned char byte;
    unsigned char bit;
    unsigned char pv;

    if(nbits == 0)
	return;
    
    nbits_1 = nbits - 1;
    nbytes = nbits_1 / 8;
    for(i = 0; i < nbytes; i++) {
	byte = buf[i];
	
	bit = byte & 0x01;
	SEND_BIT(pv, bit, 0);
	
	bit = byte & 0x02;
	SEND_BIT(pv, bit, 0);
	
	bit = byte & 0x04;
	SEND_BIT(pv, bit, 0);
	
	bit = byte & 0x08;
	SEND_BIT(pv, bit, 0);
	
	bit = byte & 0x10;
	SEND_BIT(pv, bit, 0);
	
	bit = byte & 0x20;
	SEND_BIT(pv, bit, 0);
	
	bit = byte & 0x40;
	SEND_BIT(pv, bit, 0);
	
	bit = byte & 0x80;
	SEND_BIT(pv, bit, 0);
    }
    
    remain = nbits_1 % 8;
    byte = buf[i];
    for(i = 0; i < remain; i++) {
	bit = byte & (1 << i);
	SEND_BIT(pv, bit, 0);
    }

    byte = buf[nbits / 8];
    bit = byte & (1 << (nbits_1 % 8));
    
    SEND_BIT(pv, bit, 1);
}

void jtag_shift_inout(unsigned char *ibuf, unsigned char *obuf, int nbits) {
    int i, j;
    int nbits_1;
    int nbytes;
    int tdo;
    int byteoff, bitoff;
    int remain;
    unsigned char byte, obyte;
    unsigned char bit;
    unsigned char pv;
    
    if(nbits == 0)
	return;
    
    nbits_1 = nbits - 1;
    nbytes = nbits_1 / 8;
    for(i = 0; i < nbytes; i++) {
	byte = ibuf[i];
	obyte = 0;
	
	bit = byte & 0x01;
	SEND_GET_BIT(pv, bit, 0, tdo);
	if(tdo)
	    obyte |= 0x01;
	
	bit = byte & 0x02;
	SEND_GET_BIT(pv, bit, 0, tdo);
	tdo = GET_TDO();
	if(tdo)
	    obyte |= 0x02;
	
	bit = byte & 0x04;
	SEND_GET_BIT(pv, bit, 0, tdo);
	if(tdo)
	    obyte |= 0x04;
	
	bit = byte & 0x08;
	SEND_GET_BIT(pv, bit, 0, tdo);
	if(tdo)
	    obyte |= 0x08;
	
	bit = byte & 0x10;
	SEND_GET_BIT(pv, bit, 0, tdo);
	if(tdo)
	    obyte |= 0x10;
	
	bit = byte & 0x20;
	SEND_GET_BIT(pv, bit, 0, tdo);
	if(tdo)
	    obyte |= 0x20;
	
	bit = byte & 0x40;
	SEND_GET_BIT(pv, bit, 0, tdo);
	if(tdo)
	    obyte |= 0x40;
	
	bit = byte & 0x80;
	SEND_GET_BIT(pv, bit, 0, tdo);
	if(tdo)
	    obyte |= 0x80;

	obuf[i] = obyte;
    }
    
    remain = nbits_1 % 8;
    byte = ibuf[i];
    obyte = 0;
    for(j = 0; j < remain; j++) {
	bit = byte & (1 << j);
	SEND_GET_BIT(pv, bit, 0, tdo);
	if(tdo)
	    obyte |= 1 << j;
	else
	    obyte &= ~(1 << j);
    }
    obuf[i] = obyte;

    byte = ibuf[nbits / 8];
    bit = byte & (1 << (nbits_1 % 8));

    SEND_GET_BIT(pv, bit, 1, tdo);
    if(tdo)
	obuf[nbits / 8] |= 1 << j;
    else
	obuf[nbits / 8] &= ~(1 << j);
}

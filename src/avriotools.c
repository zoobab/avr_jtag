#include "avriotools.h"
#include <avr/io.h>

typedef struct _port_regs {
    volatile uint8_t *port, *ddr, *pin;
} port_regs_t;

static port_regs_t regs[] = {
    { &PORTB, &DDRB, &PINB },
    { &PORTD, &DDRD, &PIND }
};
#define NPORTS (sizeof(regs) / sizeof(*regs))

#define ERR -1
#define OK 0

int pin_mode(volatile uint8_t * port, int pin, pin_mode_t mode) {
    port_regs_t *preg;
    int i;

    for(i = 0; i < NPORTS; i++) {
	preg = regs + i;
	if(preg->port == port)
	    break;
    }
    if(i == NPORTS)
	return ERR;

    switch(mode) {
    case PM_INPUT:
	*preg->ddr &= ~_BV(pin);
	*preg->port |= _BV(pin);
	break;
	
    case PM_OUTPUT:
	*preg->ddr |=  _BV(pin);
	break;
	
    default:
	return ERR;
    }

    return OK;
}

/* To make setbaud.h no more complaint */
#define BAUD 9600
#include <util/setbaud.h>

int uart_init(uint32_t baud) {
    unsigned long ubrr_v;
    long err;

    if(baud > (F_CPU / 8))
	return ERR;

#define F_CPUx2 (F_CPU * 2UL)

    ubrr_v = (F_CPU + baud * 8UL) / (baud * 16UL) - 1UL;
    /* Allow bias of baud rate in 2%, or use U2X clock.
     * ATmega168 divides baud rate generator clock by 2, 8, or 16.
     * Setting U2X bit makes the clock divided by 8 instead of 16.
     * It makes more accurately output and higher max baud rate.
     */
    err = 16L * 100L * (ubrr_v + 1) * baud - F_CPU * 100L;
    if(err > F_CPUx2 || err < -F_CPUx2 || ubrr_v < 2) {
	ubrr_v = (F_CPU + baud * 4UL) / (baud * 8UL) - 1UL;
	UCSRA |= (1 << U2X);
    } else {
	UCSRA &= ~(1 << U2X);
    }
    
    UBRRH = ubrr_v >> 8;
    UBRRL = ubrr_v & 0xff;
    return OK;
}

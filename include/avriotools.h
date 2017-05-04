#ifndef __AVRIOTOOLS_H_
#define __AVRIOTOOLS_H_
#include <stdint.h>
#include <avr/io.h>

typedef enum {PM_INPUT, PM_OUTPUT} pin_mode_t;

extern int pin_mode(volatile uint8_t * port, int pin, pin_mode_t mode);
#define pin_hi(port, pin) do { port |= _BV(pin); } while(0)
#define pin_lo(port, pin) do { port &= ~_BV(pin); } while(0)


#define UBRRH UBRR0L
#define UBRRL UBRR0L
#define UCSRA UCSR0A
#define U2X U2X0
#define UDRE UDRE0
#define UDR UDR0
#define RXC RXC0
#define FE FE0
#define DOR DOR0

extern int uart_init(uint32_t baud);
#define uart_getc(c)				\
    do {					\
	loop_until_bit_is_set(UCSRA, RXC);	\
	if (UCSRA & _BV(FE))			\
	    continue;				\
	if (UCSRA & _BV(DOR))			\
	    continue;				\
	c = UDR;				\
    } while(0)
/*
 * c == -1 if not data been read.
 * \note c must be an integer.
 */
#define uart_getc_nowait(c)			\
    do {					\
	c = -1;					\
	if(bit_is_clear(UCSRA, RXC)) break;	\
	if (UCSRA & _BV(FE))			\
	    break;				\
	if (UCSRA & _BV(DOR))			\
	    break;				\
	c = UDR;				\
    } while(0)
#define uart_putc(c)				\
    do {					\
	loop_until_bit_is_set(UCSRA, UDRE);	\
	UDR = ((uint8_t)(c & 0xff));		\
    } while(0)
/*
 * c == -1 if it been wrote out.
 * \note c must be an integer.
 */
#define uart_putc_nowait(c)			\
    do {					\
	if(bit_is_clear(UCSRA, UDRE))		\
	    break;				\
	UDR = ((uint8_t)(c & 0xff));		\
	c = -1;					\
    } while(0)


#endif /* __AVRIOTOOLS_H_ */

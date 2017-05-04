#ifndef __JTAG_H_
#define __JTAG_H_

#include <avr/io.h>

#define JTAG_PORT PORTB
#define JTAG_PIN PINB
#define JTAG_TCK PINB0
#define JTAG_TMS PINB1
#define JTAG_TDI PINB2
#define JTAG_TDO PINB3
#define JTAG_TRST PINB4

extern void jtag_init(void);
extern void jtag_trst(void);
extern void jtag_tms(unsigned char *buf, int nbits);
extern void jtag_shift(unsigned char *buf, int nbits);
extern void jtag_shift_inout(unsigned char *ibuf, unsigned char *obuf,
			     int nbits);

#endif /* __JTAG_H_ */

#include <stdio.h>
#include "avriotools.h"
#include <util/delay.h>
#include <avr/io.h>

int main(int argc, char *const argv[]) {
    int i, cnt = 0;
    const char *msg = "hello%d\n";
    char buf[64];
    
    uart_init(400000);
    
    pin_mode(&PORTB, PINB0, PM_OUTPUT);
    pin_mode(&PORTB, PINB1, PM_OUTPUT);
    pin_lo(PORTB, PINB1);
    for(i = 0; i < 30; i++) {
	pin_hi(PORTB, PINB0);
	_delay_ms(100);
	pin_lo(PORTB, PINB0);
	_delay_ms(100);
    }
    
    while(1) {
	pin_hi(PORTB, PINB0);
	_delay_ms(100);
	pin_lo(PORTB, PINB0);
	_delay_ms(100);
	pin_hi(PORTB, PINB0);
	_delay_ms(100);
	pin_lo(PORTB, PINB0);
	_delay_ms(100);
	pin_hi(PORTB, PINB0);
	_delay_ms(500);
	pin_lo(PORTB, PINB0);
	_delay_ms(100);
	
	sprintf(buf, msg, cnt++);
	for(i = 0; buf[i] != 0; i++)
	    uart_putc(buf[i]);
    }
}

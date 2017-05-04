#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "avriotools.h"
#include "cmd_proto.h"

#define BAUD_RATE 420000

void flash_init(void) {
    pin_mode(&PORTB, PINB0, PM_OUTPUT);
    pin_mode(&PORTB, PINB1, PM_OUTPUT);
    pin_lo(PORTB, PINB0);
    pin_lo(PORTB, PINB1);
}

void flash_led(void) {
    pin_hi(PORTB, PINB0);
    _delay_ms(50);
    pin_lo(PORTB, PINB0);
}

int main(int argc, char * const argv[]) {
    int c;
    cmd_proto_t *cp;
    cp_cmd_t *cmd;
    char buf[255 + CP_CMD_OVERHEAD];
    int sz;
    int i;
    
    flash_init();
    
    uart_init(BAUD_RATE);
    
    cp = cmd_proto_new();
    
    while(1) {
	uart_getc(c);
	cmd = cmd_proto_rcv(cp, c);
	if(cmd == NULL)
	    continue;

	flash_led();
	
	if(cmd == &BAD_CMD) {
	    memcpy(buf + CP_CMD_HEAD_SZ, "bad cmd", 7);
	    sz = cmd_proto_cmd_fill(buf, cmd->seq, CPCMD_NAK, 7);
	    for(i = 0; i < sz; i++)
		uart_putc(buf[i]);
	    continue;
	}
	
	if(cmd == &CSUM_ERR_CMD) {
	    memcpy(buf + CP_CMD_HEAD_SZ, "csum err", 8);
	    sz = cmd_proto_cmd_fill(buf, cmd->seq, CPCMD_NAK, 8);
	    for(i = 0; i < sz; i++)
		uart_putc(buf[i]);
	    continue;
	}
	
	switch(cmd->code) {
	case CPCMD_PING:
	    memcpy(buf + CP_CMD_HEAD_SZ, cmd->data, cmd->data_sz);
	    sz = cmd_proto_cmd_fill(buf, cmd->seq, CPCMD_PONG, cmd->data_sz);
	    for(i = 0; i < sz; i++)
		uart_putc(buf[i]);
	    break;

	default:
	    memcpy(buf + CP_CMD_HEAD_SZ, cmd->data, cmd->data_sz);
	    sz = cmd_proto_cmd_fill(buf, cmd->seq, CPCMD_DATA, cmd->data_sz);
	    for(i = 0; i < sz; i++)
		uart_putc(buf[i]);
	}
    }
}

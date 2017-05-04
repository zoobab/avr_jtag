#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "avriotools.h"
#include "cmd_proto.h"
#include "jtag.h"

#define BAUD_RATE 420000

static unsigned char client_buf[256 + CP_CMD_OVERHEAD];

static
void ack(int seq) {
    int i;
    int sz;
    
    sz = cmd_proto_cmd_fill(client_buf, seq, CPCMD_ACK, 0);
    
    for(i = 0; i < sz; i++)
	uart_putc(client_buf[i]);
}

static
void nak(int seq, const unsigned char *msg) {
    int i;
    int sz;
    
    memcpy(client_buf + CP_CMD_HEAD_SZ, msg, strlen(msg));
    sz = cmd_proto_cmd_fill(client_buf, seq, CPCMD_NAK, 7);
    
    for(i = 0; i < sz; i++)
	uart_putc(client_buf[i]);
}

static
void send_client(const unsigned char *buf, int bsz) {
    int i;
    
    for(i = 0; i < bsz; i++)
	uart_putc(buf[i]);
}

#define GET_DATA_BITS(data) (((unsigned int)(data)[0]) |	\
			     ((unsigned int)(data)[1] << 8))

int main(int argc, char * const argv[]) {
    cp_cmd_t *cmd;
    cmd_proto_t *cp;
    int c;
    unsigned int nbits;
    int bsz;
    static unsigned char buf[16];
    
    uart_init(BAUD_RATE);
    jtag_init();
    cp = cmd_proto_new();

    while(1) {
	uart_getc(c);
	cmd = cmd_proto_rcv(cp, c);
	if(cmd == NULL)
	    continue;

	if(cmd == &BAD_CMD) {
	    nak(cmd->seq, "BAD CMD");
	    continue;
	}
	
	if(cmd == &CSUM_ERR_CMD) {
	    nak(cmd->seq, "CSUM ERR");
	    continue;
	}

	switch(cmd->code) {
	case CPCMD_PING:
	    memcpy(client_buf + CP_CMD_HEAD_SZ, cmd->data, cmd->data_sz);
	    bsz = cmd_proto_cmd_fill(client_buf, cmd->seq,
				     CPCMD_PONG, cmd->data_sz);
	    send_client(client_buf, bsz);
	    break;
	    
	case CPCMD_SHIFT_TDI:
	    nbits = GET_DATA_BITS(cmd->data);
	    jtag_shift(cmd->data + 2, nbits);
	    ack(cmd->seq);
	    break;
	    
	case CPCMD_SHIFT_TMS:
	    nbits = GET_DATA_BITS(cmd->data);
	    jtag_tms(cmd->data + 2, nbits);
	    ack(cmd->seq);
	    break;
	    
	case CPCMD_SHIFT_TDI_TDO:
	    nbits = GET_DATA_BITS(cmd->data);
	    jtag_shift_inout(cmd->data + 2,
			     client_buf + CP_CMD_HEAD_SZ + 2,
			     nbits);
	    
	    client_buf[CP_CMD_HEAD_SZ] = nbits & 0xff;
	    client_buf[CP_CMD_HEAD_SZ + 1] = nbits >> 8;
	    
	    bsz = cmd_proto_cmd_fill(client_buf, cmd->seq,
				     CPCMD_DATA, (nbits + 23) / 8);
	    send_client(client_buf, bsz);
	    break;
	    
	case CPCMD_TRST:
	    jtag_trst();
	    bsz = cmd_proto_cmd_fill(client_buf, cmd->seq,
				     CPCMD_ACK, 0);
	    send_client(client_buf, bsz);
	    break;
	    
	default:
	    sprintf(buf, "CODE 0x%02x", cmd->code);
	    nak(cmd->seq, buf);
	}
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd_proto.h"

enum {
    ST_WAIT_MAGIC0,
    ST_WAIT_MAGIC1,
    ST_WAIT_SEQ,
    ST_WAIT_LEN,
    ST_WAIT_DATA,
    ST_WAIT_CSUM
};

#define BUF_SZ 512

cmd_proto_t *cmd_proto_new(void) {
    cmd_proto_t *cp;

    cp = (cmd_proto_t *)malloc(sizeof(cmd_proto_t) + BUF_SZ);
    if(cp == NULL)
	return NULL;
    
    memset(cp, 0, sizeof(cmd_proto_t));
    cp->bufs[0] = ((char *)cp) + sizeof(cmd_proto_t);
    cp->bufs[1] = ((char *)cp) + sizeof(cmd_proto_t) + 256;
    cp->cmds[0].data = cp->bufs[0] + 1;
    cp->cmds[1].data = cp->bufs[1] + 1;

    return cp;
}

void cmd_proto_free(cmd_proto_t *cp) {
    free(cp);
}

cp_cmd_t BAD_CMD;
cp_cmd_t CSUM_ERR_CMD;

static
cp_cmd_t *make_cmd(cmd_proto_t *cp) {
    char *buf = cp->bufs[cp->receiving];
    cp_cmd_t *cmd;
    int i;

    if(buf[0] <= 0 || buf[0] >= CPCMD_MAX)
	return &BAD_CMD;
    
    cmd = cp->cmds + cp->receiving;

    cmd->seq = cp->seq;
    cmd->code = buf[0];
    cmd->data_sz = cp->len - 1;

    cp->receiving ^= 1;
    
    return cmd;
}

#define CSUM_ADD(csum, c)					\
    do {							\
	csum = ((((csum) << 3) | ((csum) >> 5)) ^ (c)) & 0xff;	\
    } while(0)

cp_cmd_t *cmd_proto_rcv(cmd_proto_t *cp, int c) {
    int i, csum;
    char *buf;
    cp_cmd_t *cmd = NULL;
    
    switch(cp->status) {
    case ST_WAIT_MAGIC0:
	if(c == CP_MAGIC[0])
	    cp->status = ST_WAIT_MAGIC1;
	break;
	
    case ST_WAIT_MAGIC1:
	if(c == CP_MAGIC[1])
	    cp->status = ST_WAIT_SEQ;
	else if(c != CP_MAGIC[0])
	    cp->status = ST_WAIT_MAGIC0;
	break;

    case ST_WAIT_SEQ:
	cp->seq = c;
	cp->status = ST_WAIT_LEN;
	break;
	
    case ST_WAIT_LEN:
	cp->len = c;
	if(c > 0) {
	    cp->status = ST_WAIT_DATA;
	    cp->cnt = 0;
	} else
	    cp->status = ST_WAIT_CSUM;
	break;
	
    case ST_WAIT_DATA:
	cp->bufs[cp->receiving][cp->cnt++] = c;
	if(cp->cnt >= cp->len)
	    cp->status = ST_WAIT_CSUM;
	break;
	
    case ST_WAIT_CSUM:
	csum = 0;
	buf = cp->bufs[cp->receiving];
	for(i = 0; i < cp->len; i++)
	    CSUM_ADD(csum, buf[i]);
	CSUM_ADD(csum, c);
	cp->status = ST_WAIT_MAGIC0;
	if(csum == 0)
	    cmd = make_cmd(cp);
	else
	    cmd = &CSUM_ERR_CMD;
	break;
	
    default:
	cp->status = ST_WAIT_MAGIC0;
    }

    return cmd;
}

int cmd_proto_cmd_fill(unsigned char *buf, int seq,
		       cp_ccode_t code, int data_sz) {
    int i, last = data_sz + CP_CMD_HEAD_SZ;
    int csum = 0;
    
    buf[0] = CP_MAGIC[0];
    buf[1] = CP_MAGIC[1];
    buf[2] = seq;
    buf[3] = data_sz + 1;
    buf[4] = code;
    for(i = CP_CMD_HEAD_SZ - 1; i < last; i++)
	CSUM_ADD(csum, buf[i]);
    CSUM_ADD(csum, 0);
    buf[i] = csum;
    
    return data_sz + CP_CMD_OVERHEAD;
}

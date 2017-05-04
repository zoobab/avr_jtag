#ifndef __CMD_PROTO_H_
#define __CMD_PROTO_H_

#define CP_MAGIC "JC"
#define CP_MAX_SZ 0x255
typedef enum {
    CPCMD_DUMMY,
    CPCMD_PING,
    CPCMD_SHIFT_TDI,
    CPCMD_SHIFT_TMS,
    CPCMD_SHIFT_TDI_TDO,
    CPCMD_TRST,
    CPCMD_PONG,
    CPCMD_DATA,
    CPCMD_ACK,
    CPCMD_NAK,
    CPCMD_MAX
} cp_ccode_t;

typedef struct {
    int seq;
    cp_ccode_t code;
    int data_sz;
    unsigned char *data;
} cp_cmd_t;

typedef struct {
    cp_cmd_t cmds[2];
    int receiving;
    int status;
    int seq;
    int cnt;
    int len;
    unsigned char *bufs[2];
} cmd_proto_t;

extern cmd_proto_t *cmd_proto_new(void);
extern void cmd_proto_free(cmd_proto_t *cp);
extern cp_cmd_t *cmd_proto_rcv(cmd_proto_t *cp, int c);
/*
 * \return size of filled command.
 */
extern int cmd_proto_cmd_fill(unsigned char *buf, int seq,
			      cp_ccode_t code, int data_sz);
extern cp_cmd_t BAD_CMD;
extern cp_cmd_t CSUM_ERR_CMD;

#define CP_CMD_HEAD_SZ 5
#define CP_CMD_TAIL_SZ 1
#define CP_CMD_OVERHEAD (CP_CMD_HEAD_SZ + CP_CMD_TAIL_SZ)

#endif /* __CMD_PROTO_H_ */

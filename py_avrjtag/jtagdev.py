import tms_ptns
import cmd_proto

debug_frame = 0

def dump_frame(frame):
    r = ''
    for c in frame:
        r = r + ('%02x ' % (ord(c)))
        pass
    print r
    pass

class jtagdev(object):
    ST_RESET = 0
    ST_IDLE = 1
    ST_SHIFT_DR = 2
    ST_SHIFT_IR = 3
    ST_EXIT1_DR = 4
    ST_EXIT1_IR = 5
    
    def __init__(self, fo):
        self.fo = fo
        self.state = self.ST_RESET
        self.seq = 0
        pass

    def _send_cmd(self, code, data):
        cmd = cmd_proto.cmd(self.seq, code, data)
        frame = cmd.to_frame()

        self.fo.write(frame)
        self.fo.flush()
        pass

    def _send_nbits_data_cmd(self, code, nbits, data):
        nbits_data = cmd_proto.prepend_nbits(nbits, data)
        self._send_cmd(code, nbits_data)
        pass

    def _send_tms(self, ptn, nbits):
        self._send_nbits_data_cmd(cmd_proto.CPCMD_SHIFT_TMS,
                                  nbits, ptn)
        pass

    def wait_reply(self, tmo=None):
        import select
        
        frame = ''
        while True:
            if tmo == None:
                rlist, wlist, xlist = select.select((self.fo,), (), ())
            else:
                rlist, wlist, xlist = select.select((self.fo,), (), (), tmo)
                pass
            if not rlist:
                break
            frame = frame + self.fo.read()
            if len(frame) >= 4 and \
                    len(frame) >= (ord(frame[3]) +
                                   cmd_proto.cmd.FRAME_OVERHEAD):
                    break
            tmo = 0.05
            pass
        
        if not frame:
            return None
        if debug_frame:
            dump_frame(frame)
            pass
        
        cmd = cmd_proto.cmd()
        r = cmd.from_frame(frame)
        if r:
            return None
        return cmd

    def idle(self):
        self.seq = (self.seq + 1) % 256
        
        if self.state == self.ST_RESET:
            ptn, nbits = tms_ptns.TMS_RESET_2_IDLE
        else:
            ptn, nbits = tms_ptns.TMS_IDLE_SEQ
            pass
        self._send_tms(ptn, nbits)
        
        self.state = self.ST_IDLE
        pass

    def go_shift_IR(self):
        if self.state == self.ST_SHIFT_IR:
            return
        
        if self.state == self.ST_IDLE:
            ptn, ptn_nbits = tms_ptns.TMS_SHIFT_IR
        elif self.state == self.ST_EXIT1_DR:
            ptn, ptn_nbits = tms_ptns.TMS_EXIT1_DR_2_SHIFT_IR
        elif self.state == self.ST_EXIT1_IR:
            ptn, ptn_nbits = tms_ptns.TMS_EXIT1_IR_2_SHIFT_IR
        else:
            raise RuntimeError, 'Transite to shift IR state from invalid state'
        self._send_tms(ptn, ptn_nbits)
        self.state = self.ST_SHIFT_IR
        pass

    def go_shift_DR(self):
        if self.state == self.ST_SHIFT_DR:
            return
        
        if self.state == self.ST_IDLE:
            ptn, ptn_nbits = tms_ptns.TMS_SHIFT_DR
        elif self.state == self.ST_EXIT1_IR:
            ptn, ptn_nbits = tms_ptns.TMS_EXIT1_IR_2_SHIFT_DR
        elif self.state == self.ST_EXIT1_DR:
            ptn, ptn_nbits = tms_ptns.TMS_EXIT1_DR_2_SHIFT_DR
        else:
            raise RuntimeError, 'Transite to shift DR state from invalid state'
            pass
        self._send_tms(ptn, ptn_nbits)
        self.state = self.ST_SHIFT_DR
        pass

    def shift_IR(self, data, nbits):
        self.seq = (self.seq + 1) % 256

        if self.state != self.ST_SHIFT_IR:
            raise RuntimeError, 'Invalid state'

        self._send_nbits_data_cmd(cmd_proto.CPCMD_SHIFT_TDI,
                                  nbits, data)
        self.state = self.ST_EXIT1_IR
        pass

    def shift_IR_n_out(self, data, nbits):
        self.seq = (self.seq + 1) % 256

        if self.state != self.ST_SHIFT_IR:
            raise RuntimeError, 'Invalid state'

        self._send_nbits_data_cmd(cmd_proto.CPCMD_SHIFT_TDI_TDO,
                                  nbits, data)
        self.state = self.ST_EXIT1_IR
        pass

    def shift_DR(self, data, nbits):
        self.seq = (self.seq + 1) % 256

        if self.state != self.ST_SHIFT_DR:
            raise RuntimeError, 'Invalid state'

        self._send_nbits_data_cmd(cmd_proto.CPCMD_SHIFT_TDI,
                                  nbits, data)
        self.state = self.ST_EXIT1_DR
        pass

    def shift_DR_n_out(self, data, nbits):
        self.seq = (self.seq + 1) % 256

        if self.state != self.ST_SHIFT_DR:
            raise RuntimeError, 'Invalid state'

        self._send_nbits_data_cmd(cmd_proto.CPCMD_SHIFT_TDI_TDO,
                                  nbits, data)
        self.state = self.ST_EXIT1_DR
        pass

    def reset(self):
        self._send_cmd(cmd_proto.CPCMD_TRST, '')
        self.state = self.ST_RESET
        pass
    pass

def _extract_nbits_data(nbits_data):
    nbits = ord(nbits_data[0]) | (ord(nbits_data[1]) << 8)
    return nbits, nbits_data[2:]

def _get_bit(data, bit_idx):
    byte_off = bit_idx / 8
    bit_off = bit_idx % 8
    return (ord(data[byte_off]) >> bit_off) & 0x1

def identify_components(dev):
#     dev.go_shift_IR()
#     reply = dev.wait_reply()
#     if reply.code != cmd_proto.CPCMD_ACK:
#         raise RuntimeError, 'invalid reply code 0x%02x' % (reply.code)
    
#     dev.shift_IR('\x04', 4)
#     reply = dev.wait_reply()
#     if reply.code != cmd_proto.CPCMD_ACK:
#         raise RuntimeError, 'invalid reply code 0x%02x' % (reply.code)

    dev.go_shift_DR()
    reply = dev.wait_reply()
    if reply.code != cmd_proto.CPCMD_ACK:
        raise RuntimeError, 'invalid reply code 0x%02x' % (reply.code)
    
    dev.shift_DR_n_out('\xff' * 252, 252 * 8)
    reply = dev.wait_reply()
    if not reply:
        raise RuntimeError, 'Invalid replied message'
    if reply.code != cmd_proto.CPCMD_DATA:
        raise RuntimeError, 'invalid replied code 0x%02x %s' % (reply.code, repr(reply))

    nbits, data = _extract_nbits_data(reply.data)
    
    components = []
    i = 0
    while i < nbits:
        bit = _get_bit(data, i)
        if bit == 0:
            components.append(0)
            i = i + 1
        else:
            comp_id = 0
            for j in range(32):
                bit = _get_bit(data, i + j)
                comp_id = comp_id | (bit << j)
                pass
            if comp_id == 0xffffffff:
                break
            components.append(comp_id)
            i = i + 32
            pass
        pass
    return components

import struct

CPCMD_PING = 1
CPCMD_SHIFT_TDI = 2
CPCMD_SHIFT_TMS = 3
CPCMD_SHIFT_TDI_TDO = 4
CPCMD_TRST = 5
CPCMD_PONG = 6
CPCMD_DATA = 7
CPCMD_ACK = 8
CPCMD_NAK = 9

def csum_add(csum, c):
    return (((csum << 3) | (csum >> 5)) ^ c) & 0xff

class cmd(object):
    OFF_SEQ = 2
    OFF_DSZ = 3
    OFF_CODE = 4
    FRAME_OVERHEAD = 5
    
    def __init__(self, seq=0, code=0, data=''):
        if len(data) >= 255:
            raise ValueError, 'data length (%d) >= 255' % (len(data))
        self.seq = seq
        self.code = code
        self.data = data
        pass
    
    def to_frame(self):
        csum = 0
        csum = csum_add(0, self.code)
        for c in self.data:
            csum = csum_add(csum, ord(c))
            pass
        csum = csum_add(csum, 0)
        
        frame = struct.pack('BBBBB', ord('J'), ord('C'), self.seq,
                            len(self.data) + 1, self.code)
        frame = frame + self.data + chr(csum)
        return frame

    def from_frame(self, frame):
        if frame[:2] != 'JC':
            return -1
        
        csum = 0
        for c in frame[self.OFF_CODE:]:
            csum = csum_add(csum, ord(c))
            pass
        if csum:
            return -1
        
        data_sz = ord(frame[self.OFF_DSZ])
        if data_sz != (len(frame) - self.FRAME_OVERHEAD):
            return -1

        if not data_sz:
            return -1

        self.seq = ord(frame[self.OFF_SEQ])
        self.code = ord(frame[self.OFF_CODE])
        self.data = frame[self.OFF_CODE + 1:-1]
        pass

    def __repr__(self):
        return '<%s {seq: %d, code: %d, data: %s}>' % \
            (self.__class__.__name__, self.seq, self.code, repr(self.data))
    pass

def prepend_nbits(nbits, data):
    r = chr(nbits & 0xff) + chr(nbits >> 8) + data
    return r

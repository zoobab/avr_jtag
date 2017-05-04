import cmd_proto

def cp_ping(seq, data):
    cmd = cmd_proto.cmd(seq, cmd_proto.CPCMD_PING, data)
    frame = cmd.to_frame()
    return frame

def get_reply(fo):
    import fcntl, os
    import time
    
    reply = ''
    fcntl.fcntl(fo.fileno(), fcntl.F_SETFL, os.O_NONBLOCK)
    while True:
        try:
            s = os.read(fo.fileno(), 256)
        except OSError:
            time.sleep(0.5)
            try:
                s = os.read(fo.fileno(), 256)
            except OSError:
                break
            pass
        reply = reply + s
        pass
    return reply

if __name__ == '__main__':
    import sys

    if len(sys.argv) != 2:
        print >> sys.stderr, 'Usage: prog <port device>'
        sys.exit(1)
        pass
    
    port = sys.argv[1]
    
    fo = open(port, 'r+b')
    cmd_str = cp_ping(20, 'hello')
    fo.write(cmd_str)
    fo.flush()
    
    frame = get_reply(fo)
    cmd = cmd_proto.cmd()
    cmd.from_frame(frame)
    print repr(cmd)


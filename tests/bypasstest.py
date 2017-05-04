import sys
import os
import fcntl
import jtagdev

if len(sys.argv) != 2:
    print >>sys.stderr, 'Usage: %s <UART Port>' % (sys.argv[0])
    sys.exit(1)
uart_fname = sys.argv[1]

try:
    uart_fo = file(uart_fname, 'r+b')
except IOError, e:
    print e
    sys.exit(1)
    pass

flags = fcntl.fcntl(uart_fo, fcntl.F_GETFL)
fcntl.fcntl(uart_fo, fcntl.F_SETFL, os.O_NONBLOCK | flags)

jtagdev.debug_frame = 1

dev = jtagdev.jtagdev(uart_fo)

dev.reset()
dev.wait_reply()

dev.idle();
dev.wait_reply()

dev.go_shift_IR()
dev.wait_reply()

dev.shift_IR_n_out('\x0f', 4)
dev.wait_reply()

dev.go_shift_DR()
dev.wait_reply()

dev.shift_DR_n_out('\x01' * 128, 1024)
dev.wait_reply()

dev.go_shift_DR()
dev.wait_reply()

dev.shift_DR_n_out('\xff\xff', 16)
dev.wait_reply()

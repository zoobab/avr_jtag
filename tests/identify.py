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

dev = jtagdev.jtagdev(uart_fo)

print 'Reset:',
dev.reset()
reply = dev.wait_reply()
print reply

print 'Go idle:',
dev.idle()
reply = dev.wait_reply()
print reply

print 'Identify components:',
components = jtagdev.identify_components(dev)
for comp in components:
    print '%08x' % (comp)
    pass


CFLAGS =	-DF_CPU=16000000UL -mmcu=atmega168 -I../include -O
AVR_JTAG_OBJS =	\
	avr_jtag.o	\
	avriotools.o	\
	cmd_proto.o	\
	jtag.o
CC= avr-gcc
OBJCOPY= avr-objcopy
PORT?= /dev/cuaU0

all: avr_jtag.hex

avr_jtag.hex: avr_jtag
	$(OBJCOPY) -O ihex -R .eeprom $(.ALLSRC) $@

avr_jtag: $(AVR_JTAG_OBJS:C/\.o/.c/)
	$(CC) $(CFLAGS) -o $@ $(.ALLSRC)

$(AVR_JTAG_OBJS): ${@:C/\.o/.c/}
	$(CC) $(CFLAGS) -c $(.ALLSRC)

install:
	avrdude -V -F -c stk500v1 -p m168 -b 19200 -P $(PORT) \
		-U flash:w:avr_jtag.hex


clean:
	@for f in $(AVR_JTAG_OBJS) avr_jtag avr_jtag.hex *~; do \
		if [ -e $$f ]; then \
			echo "delete $$f"; \
			rm -f $$f; \
		fi; \
	done

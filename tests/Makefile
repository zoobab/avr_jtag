HEXS :=	ledtest.hex cptest.hex
CC := avr-gcc
OBJCOPY := avr-objcopy
CFLAGS :=	-DF_CPU=16000000UL -mmcu=atmega168 -I../include -O
PORT?= /dev/cuaU0

all: $(HEXS)

ledtest.hex: ledtest
	$(OBJCOPY) -O ihex -R .eeprom $(.ALLSRC) $@

cptest.hex: cptest
	$(OBJCOPY) -O ihex -R .eeprom $(.ALLSRC) $@

cptest: cptest.c ../src/cmd_proto.c ../src/avriotools.c
	$(CC) $(CFLAGS) -o $@ $(.ALLSRC)

ledtest: ledtest.c ../src/avriotools.c
	$(CC) $(CFLAGS) -o $@ $(.ALLSRC)

install-ledtest:
	avrdude -V -F -c stk500v1 -p m168 -b 19200 -P $(PORT) \
		-U flash:w:ledtest.hex

install-cptest:
	avrdude -V -F -c stk500v1 -p m168 -b 19200 -P $(PORT) \
		-U flash:w:cptest.hex

clean:
	@for f in $(HEXS) ${HEXS:C/\.hex//} *~; do \
		if [ -e $$f ]; then \
			echo "delete $$f"; \
			rm -f $$f; \
		fi; \
	done

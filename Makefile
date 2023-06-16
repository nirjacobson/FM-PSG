MMCU          = atmega644p
DEVICE        = m644p
SERIAL_PORT   = /dev/ttyUSB0
PROGRAMMER    = avrisp2 
LFUSE         = 0xE6
HFUSE         = 0xD9

MODULES  = main
OBJECTS  = $(foreach MODULE, ${MODULES}, build/${MODULE}.o)
CFLAGS   = -Wall -O2 -std=c17 --param=min-pagesize=0
LDFLAGS  = 
EXEC     = fm-psg
EXEC_HEX = ${EXEC}.hex

${EXEC_HEX}: ${EXEC}
	avr-objcopy -j .text -j .data -O ihex $< $@

${EXEC}: ${OBJECTS}
	avr-gcc -mmcu=${MMCU} $^ -o $@ ${LDFLAGS}

build/:
	mkdir -p build

build/%.o: build/ src/%.c
	avr-gcc -mmcu=${MMCU} -c $(word 2, $^) -o $@ ${CFLAGS}

upload: ${EXEC_HEX}
	avrdude -P ${SERIAL_PORT} -p ${DEVICE} -c ${PROGRAMMER} -F -e -U flash:w:${EXEC_HEX}

format:
	astyle -rnNCS *.{c,h}

clean:
	rm -rf build
	rm ${EXEC}
	rm ${EXEC_HEX}


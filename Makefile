MMCU        = atmega644p
DEVICE		= m644p
SERIAL_PORT = /dev/ttyUSB0
PROGRAMMER  = avrisp2 
LFUSE    	= 0xE6
HFUSE    	= 0xD9

MODULES = usart spi sd fat32 main
OBJECTS = $(foreach MODULE, ${MODULES}, build/${MODULE}.o)
CFLAGS  = -Wall -O2 -std=c11
LDFLAGS = 
EXEC    = fm-psg
EXEC_HEX = ${EXEC}.hex

all: build/ ${EXEC_HEX}

upload: ${EXEC_HEX}
	avrdude -P ${SERIAL_PORT} -p ${DEVICE} -c ${PROGRAMMER} -F -e -U flash:w:${EXEC_HEX}

lfuse:
	avrdude -P ${SERIAL_PORT} -p ${DEVICE} -c ${PROGRAMMER} -U lfuse:w:${LFUSE}:m

hfuse:
	avrdude -P ${SERIAL_PORT} -p ${DEVICE} -c ${PROGRAMMER} -U hfuse:w:${HFUSE}:m

read_lfuse:
	avrdude -P ${SERIAL_PORT} -p ${DEVICE} -c ${PROGRAMMER} -U lfuse:r:-:i

read_hfuse:
	avrdude -P ${SERIAL_PORT} -p ${DEVICE} -c ${PROGRAMMER} -U hfuse:r:-:i


avrdude:
	avrdude -P ${SERIAL_PORT} -p ${DEVICE} -c ${PROGRAMMER} -v

${EXEC_HEX}: ${EXEC}
	avr-objcopy -j .text -j .data -O ihex $< $@

${EXEC}: ${OBJECTS}
	avr-gcc -mmcu=${MMCU} $^ -o $@ ${LDFLAGS}

format:
	astyle -rnNCS *.{c,h}

build/%.o : src/%.c
	avr-gcc -mmcu=${MMCU} -c $< -o $@ ${CFLAGS}

%.asm: src/%.c
	avr-gcc -mmcu=${MMCU} -c $< -o $@ ${CFLAGS} -S

build/:
	mkdir -p build

clean:
	rm -rf build
	rm ${EXEC}
	rm ${EXEC_HEX}


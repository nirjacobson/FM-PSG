MMCU        = atmega644p
DEVICE		= m644p
SERIAL_PORT = /dev/ttyUSB0
PROGRAMMER  = avrisp2 
LFUSE    	= 0xE6
HFUSE     = 0xD8

MODULES = buttons usart spi sd fat32 flash stk500v2 main
OBJECTS = $(foreach MODULE, ${MODULES}, build/${MODULE}.o)
CFLAGS  = -Wall -O2 -std=c11 --param=min-pagesize=0
LDFLAGS = 
EXEC    = bootloader
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
	avr-gcc -mmcu=${MMCU} -Ttext=0xE000 $^ -o $@ ${LDFLAGS}

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


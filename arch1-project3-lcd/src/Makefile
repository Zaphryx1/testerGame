# Makefile configuration
CPU             	= msp430g2553
CFLAGS          	= -mmcu=${CPU} -Os -I../h
LDFLAGS         	= -L../lib -L/opt/ti/msp430_gcc/include/

# Switch the compiler (for the internal make rules)
CC              	= msp430-elf-gcc
AS              	= msp430-elf-gcc -mmcu=${CPU} -c

all: clock.elf

# Additional rules for files
clock.elf: main.o clock.o wdt_handler.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ -lTimer -lLcd

load: clock.elf
	msp430loader.sh $^

clean:
	rm -f *.o *.elf

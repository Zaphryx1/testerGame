COMMON_OBJECTS	=
CPU		= msp430g2553
CFLAGS		= -mmcu=${CPU} -I../h
LDFLAGS		= -L/opt/ti/msp430_gcc/include -L../lib 



CC		= msp430-elf-gcc
AS		= msp430-elf-as
AR              = msp430-elf-ar

libLcd.a: font-5x7.o lcdutils.o lcddraw.o
	$(AR) crs $@ $^

lcddraw.o: lcddraw.c lcddraw.h lcdutils.h

lcdutils.o: lcdutils.c lcdutils.h

install: libLcd.a
	mkdir -p ../h ../lib
	mv $^ ../lib
	cp *.h ../h

all: fireworks.elf

fireworks.elf: ${COMMON_OBJECTS} main.c font-5x7.o lcdutils.o lcddraw.o wdInterruptHandler.o stateMachines.o switches.o buzzer.o p2_interrupt_handler.c ../lib/libTimer.a
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ -lTimer



load: fireworks.elf
	msp430loader.sh fireworks.elf



clean:
	rm -f *.o *.elf

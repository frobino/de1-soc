CC=arm-linux-gnueabihf-gcc

all: blinker_bare_metal

blinker_bare_metal: blinker_bare_metal.o 
	${CC} blinker_bare_metal.o -o blinker_bare_metal.x

main.o: blinker_bare_metal.c
	${CC} -c blinker_bare_metal.c

clean: 
	rm -rf *.o *.x


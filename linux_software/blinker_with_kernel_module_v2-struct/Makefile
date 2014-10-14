CC=arm-linux-gnueabihf-gcc

all: tutorial_module

tutorial_module: main.o 
	${CC} -Wall main.o -o tutorial_module.x

main.o: main.c
	${CC} -Wall -c main.c

clean: 
	rm -rf *.o *.x


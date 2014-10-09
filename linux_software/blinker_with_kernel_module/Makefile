CC=arm-linux-gnueabihf-gcc

all: tutorial_module

tutorial_module: main.o 
	${CC} main.o -o tutorial_module.x

main.o: main.c
	${CC} -c main.c

clean: 
	rm -rf *.o *.x


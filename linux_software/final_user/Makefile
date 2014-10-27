CC=arm-linux-gnueabihf-gcc

all: tutorial_module

tutorial_module: main.o 
	${CC} -Wall -Wstrict-prototypes main.o -o final_user.x

main.o: main.c
	${CC} -Wall -Wstrict-prototypes -c main.c

clean: 
	rm -rf *.o *.x


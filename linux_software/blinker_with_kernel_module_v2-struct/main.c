#include <stdio.h>
#include <stdlib.h>

typedef struct blink_command{
	int is_valid;
	int speed;
} blink;

int main (int argc, char *argv[]){

	if(argc==2){
		FILE * pFile;
		// File must be opened as "w", others such as "a+" 
		// does not work (seg fault, both user and root)
		pFile = fopen ("/sys/bus/platform/drivers/blinker/blinker" , "w");
		if (pFile==NULL)
   			printf("Failure opening file\n");

		/* We remove the buffer from the file i/o (_IONBF). 
		   No buffer is used. Each I/O operation is written 
		   as soon as possible. In this case, the buffer and 
		   size parameters are ignored.*/
		unsigned char dummy;
  		setvbuf(pFile,&dummy,_IONBF,1);

		char* value= argv[1]; // [1][0]: second string [1], char 0 [0]
                printf("value: %s \n", value);

		blink my_command;
		my_command.is_valid=1;
		my_command.speed=atoi(value);

		char* value2;
		value2=(char*) &my_command;

		// The original version 
		// fprintf(pFile,"%s",value);
		fprintf(pFile,"%s",value2);
		fclose(pFile);
	}
	else
		printf("Wrong number of arguments\n");

	return 0;
}

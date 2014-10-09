#include <stdio.h>

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

		fprintf(pFile,"%s",value);
		fclose(pFile);
	}
	else
		printf("Wrong number of arguments\n");

	return 0;
}

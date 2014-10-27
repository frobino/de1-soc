#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h> //ioctl
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>




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
		//unsigned char dummy;
  		//setvbuf(pFile,&dummy,_IONBF,1);
		//setvbuf(pFile,&dummy,_IONBF,sizeof(blink));

		char* value= argv[1]; // [1][0]: second string [1], char 0 [0]
                printf("value: %s \n", value);

		blink my_command;
		my_command.is_valid=1;
		my_command.speed=atoi(value);

		printf("Sending speed %d\n",my_command.speed);

		// The original version 
		// char* value2;
                // value2=(char*) &my_command;
		// fprintf(pFile,"%s",value);
		//
		//fprintf(pFile,"%s",value2);

		void* void_pointer;
		void_pointer=(void*)&my_command;
	
		size_t sent_elements;
		sent_elements=fwrite(void_pointer,sizeof(blink),1,pFile);
		printf("Sent %d elements, sizeof blink is %d bytes\n",sent_elements,sizeof(blink));
		
		//if(!ioctl(pFile,IOCTL_GET_DATA,&my_command))
		//	printf("ioctl failed\n");

		fclose(pFile);
	}
	else
		printf("Wrong number of arguments\n");

	return 0;
}

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h> // for read, write, close
//#include <linux/ioctl.h> // for _IORD _IOWR

//#define MY_MACIG 'G'
#define MY_MACIG ']'

#define READ_IOCTL _IOR(MY_MACIG, 0, int)
#define WRITE_IOCTL _IOW(MY_MACIG, 1, int)


int main(){
	char buf[200];
	int fd = -1;
	if ((fd = open("/dev/my_device", O_RDWR)) < 0) {
		perror("open");
		return -1;
	}

	printf("READ_IOCTL user: %d\n",READ_IOCTL);
	printf("WRITE_IOCTL user: %d\n",WRITE_IOCTL);

	if(ioctl(fd, WRITE_IOCTL, "hello world") < 0)
		perror("first ioctl");
	if(ioctl(fd, READ_IOCTL, buf) < 0)
		perror("second ioctl");

	printf("message: %s\n", buf);

	/*--------------------------------------------*/

	char message[200]="Hell Yeah!";
	
	int ret;
	ret=write(fd,message,10); //tries to write 10 bytes
	if (ret==-1)
		printf("Error write\n");

	ret=read(fd,buf,10); //tries to read 10 bytes
	if (ret==-1)
		printf("Error read\n");

	buf[ret]='\0';
	
	printf("buf: %s ;length: %d bytes\n",buf,ret);
	
	close(fd);

	return 0;

}

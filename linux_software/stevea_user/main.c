#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h> //ioctl
#include <unistd.h> //sleep
#include <errno.h>
#include <string.h>

// mknod /dev/memory c 60 0

struct params
{
  int op;
  char pname[42];
};

#define IOCTL_SET_MSG 1066

int
main(int argc, char *argv[])
{
  int rc, fd;

  struct params test1 = { 1, "drop them" };
  struct params test2 = { 0, "don't drop them" };

  if( (fd = open("/sys/bus/platform/drivers/blinker/blinker", O_WRONLY)) == -1 ) {
    perror("open(\"/sys/bus/platform/drivers/blinker/blinker\", O_WRONLY)");
    exit(1);
  }

  rc = ioctl(fd, IOCTL_SET_MSG, &test1);
  printf("ioctl(fd, IOCTL_SET_MSG, &test1) = %d\n", rc);
  if(rc==-1){
	 printf("Oh dear, something went wrong with reaioctl(fd, IOCTL_SET_MSG, &test1)! %s\n", strerror(errno));
  }
  sleep(1);
  rc = ioctl(fd, IOCTL_SET_MSG, &test2);
  printf("ioctl(fd, IOCTL_SET_MSG, &test2) = %d\n", rc);
  if(rc==-1){
	 printf("Oh dear, something went wrong with reaioctl(fd, IOCTL_SET_MSG, &test2)! %s\n", strerror(errno));
  }

  close(fd);

  exit(0);
}


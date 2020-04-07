// PROGRAMA p5b.c
#include <stdio.h> 
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(void)
{
  int fd;
  char *text1="CCCCC";
  char *text2="DDDDD";

  fd = open("f1.txt", O_WRONLY|O_SYNC | O_APPEND,0600);
  write(fd,text1,5);
  write(fd,text2,5);
  close(fd);
  return 0;
}
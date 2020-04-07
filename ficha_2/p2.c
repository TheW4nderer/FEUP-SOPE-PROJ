//FOLHA 2 - p2b.c
//FILE COPY
//USAGE: copy source destination

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[])
{
 int fd1, fd2, nr, nw;
 unsigned char buffer[BUFFER_SIZE];

 //int open(const char * pathname, int oflag, ...);  /* mode_t mode *
  
 if (argc > 3) {
  printf("Usage: %s <source> <destination>\n", argv[0]);
  return 1;
  }

  fd1 = open(argv[1], O_RDONLY);
  if (fd1 == -1) { //error caught
   perror(argv[1]);
   return 2;
  }

  if (argc == 3){
    fd2 = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, 0644);
    dup2(fd2, STDOUT_FILENO);

  }
  if (fd2 == -1) { //error caught
    perror(argv[2]);
    close(fd1);
    return 3;
  
  }
  while ((nr = read(fd1, buffer, BUFFER_SIZE)) > 0)
    if ((nw = write(STDOUT_FILENO, buffer, nr)) <= 0 || nw != nr) {
        perror(argv[2]);
        close(fd1);
        close(fd2);
        return 4;
  }
  
  close(fd1);
  close(fd2);
  return 0;
}

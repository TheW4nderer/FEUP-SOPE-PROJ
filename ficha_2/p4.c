#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#define MAX_NAME_SIZE 50


int main(int argc, char* argv[]){
    
    struct termios term, oldterm;
    int fd1, nr, nw, i;
    char ch;


    if (argc != 2){
        printf("Usage: %s <source>\n", argv[0]);
        return 1;
    }



    fd1 = open(argv[1], O_WRONLY | O_CREAT, 0644);
    if (fd1 == -1){
        perror(argv[1]);
        return 1;
    }


    write(STDIN_FILENO, "Nome classificacao\n", 20);
    
    //set console to raw mode
    
    tcgetattr(STDIN_FILENO, &oldterm);
    term = oldterm;
    term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);

    i = 0;
    while (i < MAX_NAME_SIZE && read(STDIN_FILENO, &ch, 1) && ch != '/t'){ 
        write(fd1, &ch, 1);
        write(STDIN_FILENO, &ch, 1);
        i++;
    }

    //set console back to default
    tcsetattr(fd1, TCSANOW, &oldterm);
    close(fd1);
    return 0;
}
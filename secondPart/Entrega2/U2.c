#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "timer.h"
#include "utils.h"

#define BUFLENGHT 256

struct client_args args;
double elapsed_time;

int sequential = 1;
int closed = 0;

void * thr_func(void* arg){
    char* fifo = (char *) arg;
    int duration = rand() % 200 + 50; //duration between [1, 50]
    int fd = open(fifo, O_WRONLY);
    if (fd == -1) {
        closed = 1;
        display(sequential, getpid(), pthread_self(), -1, -1, CLOSD);
        return NULL;
    }    
    
    char message[BUFLENGHT];
    int idPlace = -1;

    sprintf(message,"[%d, %d, %ld, %d, %d]", sequential,getpid(), pthread_self(), duration, idPlace);
    char pfifo[BUFLENGHT] = "/tmp/"; //private fifo
    char current_string[BUFLENGHT];
    sprintf(current_string, "%d", getpid());
    strcat(pfifo, current_string);
    strcat(pfifo, ".");
    sprintf(current_string, "%ld", pthread_self());
    strcat(pfifo, current_string);

    if (mkfifo(pfifo, 0660) < 0) {
        display(sequential, getpid(), pthread_self(), duration, -1, FAILD);
        return NULL;
    }

    write(fd, &message, BUFLENGHT);
    display(sequential, getpid(), pthread_self(), duration, idPlace, IWANT); //Envio do pedido
    close(fd);

    int fd_fifo = open(pfifo, O_RDONLY);
    
    if (fd_fifo < 0) return NULL;

    char server_message[BUFLENGHT];
    if(read(fd_fifo, &server_message, BUFLENGHT) < 0){
        display(sequential, getpid(), pthread_self(), duration, -1, FAILD);
    }

    int num, pid;
    long tid;
    sscanf(server_message, "[%d, %d, %ld, %d, %d]", &num, &pid, &tid, &duration, &idPlace);

    if (duration == -1 && idPlace == -1) display(num, pid, tid, duration, idPlace, CLOSD); //Casa de banho fechada
    else display(num, pid, tid, duration, idPlace, IAMIN);

    close(fd_fifo);
    unlink(pfifo);
    return NULL;
}


int main(int argc, char* argv[]) {
    args = checkClientArgs(argc, argv);
    startClock();

    pthread_t thread;
    int t = 0;

    srand(time(NULL));
    initializeTime();

    while ((double) elapsedTime() < (double) args.nsecs){
        pthread_create(&thread, NULL, thr_func, &args.fifoname);
        pthread_detach(thread);
        usleep(10 * 1000); //tempo em ms
        t++;
        sequential++;
        if (closed) break;
    }

    return 0;
}

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
#include "log.h"

#define BUFLENGHT 256
#define MAX_THREADS 100

int nsecs;
int curr_place = 1;
//int sequential = 0;

void * thr_func(void* arg){
    int seq, pid, duration;
    long tid;
    char msg[BUFLENGHT] , pids[BUFLENGHT], tids[BUFLENGHT], fifoname[BUFLENGHT] = "/tmp/";
    strcpy(msg, (char*) arg);
    int fd_private;
    int closed = 0, place_rcv;


    sscanf(msg, "[ %d, %d, %ld, %d, %d]", &seq, &pid, &tid, &duration, &place_rcv);
    display(seq, pid, tid, duration, curr_place, RECVD);

    sprintf(pids, "%d", pid);
    strcat(fifoname, pids);
    sprintf(tids, "%ld" ,tid);
    strcat(fifoname, ".");
    strcat(fifoname, tids);
    

    if ((fd_private = open(fifoname, O_WRONLY)) < 0){
        display(seq, pid, tid, -1, -1, GAVUP);
    }

    char message[BUFLENGHT];
    if ((double) (elapsedTime() + duration * 1e-3) < (double) nsecs){
        sprintf(message, "[%d, %d, %ld, %d, %d]", seq, getpid(), pthread_self(), duration, curr_place++);
        display(seq, pid,tid, duration, curr_place, ENTER);
        
    }
    else{ 
        closed = 1;
        duration = -1;
        curr_place = -1;
        sprintf(message ,"[%d, %d, %ld, %d, %d]", seq, getpid(), (long) pthread_self(), duration, curr_place);
        display(seq, getpid(), (long) pthread_self(), -1, -1, TOOLATE);
    }    
    write(fd_private, &message, BUFLENGHT);
    usleep(duration*1000);
    if (closed == 0) display(seq, getpid(), pthread_self(), duration, curr_place, TIMUP);
    close(fd_private);
    return NULL;
}


int main(int argc, char* argv[]){
    if (argc != 4){
        printf("Usage: U1 <-t nsecs> fifoname\n");
        exit(1);
    }
    int fd;
    pthread_t tid;
    startClock();
    initializeTime();

    srand(time(NULL));

    char* fifo = argv[3];
    nsecs = atoi(argv[2]);

    if (mkfifo(fifo, 0660) < 0){
        printf("Error creating fifo\n");
    }

    fd = open(fifo, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        printf("Error reading from public fifo\n");
        exit(2);
    }


    char public_msg[BUFLENGHT];

    while(elapsedTime() < nsecs){
        if (read(fd, &public_msg, BUFLENGHT) > 0 && public_msg[0] == '['){;
            pthread_create(&tid, NULL, thr_func,  &public_msg);
            pthread_detach(tid);
        }
    }

    close(fd);

    if (unlink(fifo) < 0) printf("Unable to unlink FIFO\n");

    exit(0);


}
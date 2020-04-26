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

#define BUFLENGHT 256
#define MAX_THREADS 100

int nsecs;
int curr_place = 1;
int sequential = 0;

void * thr_func(void* arg){
    int seq, pid, duration;
    long tid;
    char msg[BUFLENGHT] , pids[BUFLENGHT], tids[BUFLENGHT], fifoname[BUFLENGHT] = "/tmp/";
    strcpy(msg, (char*) arg);
    int fd_private;

    sscanf(msg, "[ %d, %d, %ld, %d]", &seq, &pid, &tid, &duration);

    sprintf(pids, "%d", pid);
    strcat(fifoname, pids);
    sprintf(tids, "%ld" ,tid);
    strcat(fifoname, ".");
    strcat(fifoname, tids);
    printf("%s\n", fifoname);
    

    while ((fd_private = open(fifoname, O_WRONLY)) < 0){
        printf("Failed to open FIFO\n");
    }

    char message[BUFLENGHT];
    if ((double) elapsedTime() + duration * 1e-3 < (double) nsecs){
        sprintf(message, "[%d, %d, %ld, %d, %d]", sequential++, getpid(), pthread_self(), duration, curr_place++);
        printf("%s\n", message);
    }
    else sprintf(message ,"[%d, %d, %ld, %d, %d, %f]", 0, 0, (long)0, 0, 0, (float) nsecs);

    usleep(duration*1000);
    write(fd_private, &message, BUFLENGHT);
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
            pthread_join(tid, NULL);
        }
    }

    close(fd);

    if (unlink(fifo) < 0) printf("Unable to unlink FIFO\n");

    pthread_exit(0);


}
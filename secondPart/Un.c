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
char fifoname[BUFLENGHT];
double elapsed_time;


void checkArgumentsArray(char** args, int numArgs){
    nsecs = atoi(args[2]);
    strcpy(fifoname, args[3]);
    //fifoname é o canal publico
}

int sequential = 1;
int closed = 0;


void * thr_func(void* arg){
    char* fifo = (char *) arg;
    int duration = rand() % 25 +1; //duration between [1, 25]
    int fd = open(fifo, O_WRONLY);
    if (fd == -1) 
    {
        display(sequential, getpid(), pthread_self(), duration, -1, FAILD);
        return NULL;

    }    
    
    char message[BUFLENGHT];

    //generate duration randomly

    sprintf(message,"[%d, %d, %ld, %d, -1]", sequential,(int)getpid(), (long)pthread_self(), duration);
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
    display(sequential, getpid(), pthread_self(), duration, -1, IWANT); //Envio do pedido
    close(fd);

    int fd_fifo = open(pfifo, O_RDONLY);
    
    if (fd_fifo < 0) return NULL;
    char server_message[BUFLENGHT];

    int num, pid, idPlace;
    long tid;
    if(read(fd_fifo, &server_message, BUFLENGHT) < 0){
        display(sequential, getpid(), pthread_self(), duration, -1, FAILD);
    }


    sscanf(server_message, "[%d, %d, %ld, %d, %d]", &num, &pid, &tid, &duration, &idPlace);

    if (duration == -1 && idPlace == -1){ 
        display(num, pid, tid, duration, idPlace, CLOSD); //Casa de banho fechada
        closed = 1;
    }    

    else display(num, pid, tid, duration, idPlace, IAMIN);

    close(fd_fifo);

    unlink(pfifo);

    return NULL;

}


int main(int argc, char* argv[]) {
    checkArgumentsArray(argv, argc);

    startClock();

    pthread_t threads[MAX_THREADS];
    int t = 0;

    char fifo[BUFLENGHT] = "";
    strcat(fifo ,fifoname);

    srand(time(NULL));
    initializeTime();

    nsecs = atoi(argv[2]);


    while ((double) elapsedTime() < (double) nsecs){
        pthread_create(&threads[t], NULL, thr_func, &fifo);
        pthread_detach(threads[t]);
        usleep(2000000); //tempo em ms
        t++;
        sequential++;
        if (closed) break;

    }
    printf("The end !!!\n");

    //return 0;
    pthread_exit(0);
    
}
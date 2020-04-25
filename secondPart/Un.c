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
char fifoname[BUFLENGHT];
double elapsed_time;


void checkArgumentsArray(char** args, int numArgs){
    nsecs = atoi(args[2]);
    strcpy(fifoname, args[3]);
    //fifoname é o canal publico
}


void * thr_func(void* arg){
    char* fifo = arg;

    int fd = open(fifo, O_WRONLY);
    char message[BUFLENGHT];

    //generate duration randomly
    int duration = rand() % 25 +1; //duration between [1, 25]

    sprintf(message,"[%d, %ld, %d]", (int)getpid(), (long)pthread_self(), duration);
    write(fd, message, BUFLENGHT);
    close(fd);

    char pfifo[BUFLENGHT]; //private fifo
    char current_string[BUFLENGHT];
    sprintf(current_string, "%d ", getpid());
    strcpy(pfifo, current_string);
    strcat(pfifo, ".");
    sprintf(current_string, "%ld ", pthread_self());
    strcat(pfifo, current_string);

    if (mkfifo(pfifo, 0660) < 0) exit(1); //Error creating fifo
    int fd_fifo = open(pfifo, O_RDONLY);
    if (fd_fifo < 0) exit(2);
    char server_message[BUFLENGHT];

    read(fd_fifo, &server_message, BUFLENGHT);

    close(fd_fifo);

    pthread_exit(server_message);
}


int main(int argc, char* argv[]) {
    checkArgumentsArray(argv, argc);

    startClock();

    pthread_t threads[MAX_THREADS];
    int t = 0;

    char fifo[BUFLENGHT] = "";
    strcat(fifo ,fifoname);

    void* status;

    srand(time(NULL));


    while ((double) elapsedTime() < (double) nsecs){
        pthread_create(&threads[t], NULL, thr_func, &fifo);
        pthread_join(threads[t], &status);
        if (!strcmp((char*) status, "")) printf("Empty string\n");
        else printf ("%s\n", (char *)status);

        usleep(2000000);
        t++;
        printf("Seconds elapsed %f\n", elapsedTime());
    }
    pthread_exit(0);

    
}
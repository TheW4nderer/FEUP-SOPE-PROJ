#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


#define BUFLENGHT 256
#define MAX_THREADS 100

int nsecs;
char fifoname[BUFLENGHT];

double elapsedTime(){ // em MILISSEGUNDOS
    clock_t start = clock();
    return  start / (CLOCKS_PER_SEC / (double) 1000.0); 
}


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
    close(fd);
    pthread_exit(message);
}


int main(int argc, char* argv[]) {
    checkArgumentsArray(argv, argc);

    pthread_t threads[MAX_THREADS];
    int t = 0;

    char fifo[BUFLENGHT] = "";
    strcat(fifo ,fifoname);

    void* status;

    srand(time(NULL));


    while ((double) elapsedTime() < (double) nsecs){
        pthread_create(&threads[t], NULL, thr_func, &fifo);
        pthread_join(threads[t], &status);
        printf ("%s\n", (char *)status);
        usleep(2000000);
        t++;
        printf("Seconds elapsed %f\n", elapsedTime());
    }
    pthread_exit(0);

    //while(1) printf("%lf\n", elapsedTime()/1000);
    
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include "timer.h"
#include "utils.h"
#include "queue.h"

#define BUFLENGHT 256
#define MAX_THREADS 100

struct server_args args;
int places_limited = 0;
int threads_limited = 0;
int curr_place = 1;

place_queue pq;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t maxThreads;
sem_t maxPlaces;

void * thr_func(void* arg){
    int seq, pid, duration;
    long tid;
    char msg[BUFLENGHT] , pids[BUFLENGHT], tids[BUFLENGHT], fifoname[BUFLENGHT] = "/tmp/";
    int fd_private;
    int place_rcv;
    int place;

    strcpy(msg, (char*) arg);
    sscanf(msg, "[ %d, %d, %ld, %d, %d]", &seq, &pid, &tid, &duration, &place_rcv);
    display(seq, pid, tid, duration, place_rcv, RECVD);

    sprintf(pids, "%d", pid);
    strcat(fifoname, pids);
    sprintf(tids, "%ld" ,tid);
    strcat(fifoname, ".");
    strcat(fifoname, tids);
    
    if ((fd_private = open(fifoname, O_WRONLY)) < 0){
        display(seq, pid, tid, -1, -1, GAVUP);
        if (threads_limited) sem_post(&maxThreads);
        return NULL;
    }

    if (places_limited) {  //sem places_limited deveria usar na mesma o sem mas com valor INT_MAX (?)
        sem_wait(&maxPlaces);
        pthread_mutex_lock(&mutex);
        place = usePlace(&pq);
        pthread_mutex_unlock(&mutex);
    }   
    else {
        pthread_mutex_lock(&mutex);
        place = curr_place;
        curr_place++;
        pthread_mutex_unlock(&mutex);
    }

    char message[BUFLENGHT];
    if ((double) (elapsedTime() + duration * 1e-3) < (double) args.nsecs) {
        sprintf(message, "[%d, %d, %ld, %d, %d]", seq, getpid(), pthread_self(), duration, place);
        display(seq, pid, tid, duration, place, ENTER);
    }
    else{ 
        duration = -1;
        place = -1;
        sprintf(message, "[%d, %d, %ld, %d, %d]", seq, getpid(), (long) pthread_self(), duration, place);
        display(seq, getpid(), (long) pthread_self(), -1, -1, TOOLATE);
    }    

    if (write(fd_private, &message, BUFLENGHT) < 0){
        printf("Error writing response to private fifo\n");
        close(fd_private);
        if (threads_limited) sem_post(&maxThreads);
        if (places_limited){
            pthread_mutex_lock(&mutex);
            makePlaceAvailable(&pq, place);
            pthread_mutex_unlock(&mutex);
            sem_post(&maxPlaces);
        }
        return NULL;
    }

    if (close(fd_private) < 0) return NULL;

    usleep(duration * 1000);
    if (duration != -1 && place != -1) display(seq, getpid(), pthread_self(), duration, place, TIMUP);
    close(fd_private);

    if (threads_limited) sem_post(&maxThreads);
    if (places_limited){
        pthread_mutex_lock(&mutex);
        makePlaceAvailable(&pq, place);
        pthread_mutex_unlock(&mutex);
        sem_post(&maxPlaces);
    }
    return NULL;
}


int main(int argc, char* argv[]){
    int fd;
    pthread_t tid;
    startClock();
    initializeTime();
    args = checkServerArgs(argc, argv);
    if (args.nplaces != 0) places_limited = 1;
    if (args.nthreads != 0) threads_limited = 1;

    if (places_limited){
        sem_init(&maxPlaces, 0, args.nplaces);
        pq = createPlaceQueue(args.nplaces);
        fillPlaceQueue(&pq);
    }

    if (threads_limited){
        sem_init(&maxThreads, 0, args.nthreads);
    }

    srand(time(NULL));

    if (mkfifo(args.fifoname, 0660) < 0){
        printf("Error creating fifo\n");
    }

    fd = open(args.fifoname, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        printf("Error reading from public fifo\n");
        exit(2);
    }

    char public_msg[BUFLENGHT];
    while(elapsedTime() < args.nsecs){
        if (read(fd, &public_msg, BUFLENGHT) > 0 && public_msg[0] == '['){
            if (threads_limited) sem_wait(&maxThreads);
            pthread_create(&tid, NULL, thr_func,  &public_msg);
            pthread_detach(tid);
        }
    }

    close(fd);
    if (unlink(args.fifoname) < 0) printf("Unable to unlink FIFO\n");
    
    //limpar os restantes pedidos antes de terminar o programa
    while ((read(fd, &public_msg, BUFLENGHT) > 0 && public_msg[0] == '[')){
        if (threads_limited) sem_wait(&maxThreads);
        pthread_create(&tid, NULL, thr_func,  &public_msg);
        pthread_detach(tid);
    }

    return 0;
}

#include "timer.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct timeval startTime;
struct timeval currentTime;

double elapsed_time;

void startClock() {
    gettimeofday(&startTime, NULL);
    elapsed_time = 0;
}

double elapsedTime() {
    gettimeofday(&currentTime, NULL);

    elapsed_time = (currentTime.tv_sec - startTime.tv_sec) * 1e6;
    elapsed_time = (elapsed_time + (currentTime.tv_usec - startTime.tv_usec)) * 1e-6;

    return elapsed_time;
}
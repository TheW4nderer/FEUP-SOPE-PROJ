#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>




typedef enum {CREATE, EXIT, RECV_SIGNAL, SEND_SIGNAL, RECV_PIPE, SEND_PIPE, ENTRY} Action;


typedef struct{
    double instant;
    pid_t pid;
    Action action;
    char info[200];

} Log;


void startLog();

void writeToFile(Log* log);

void createLog(Action action, Log* log);

void regCreate(int argc, char* argv[]);

void regExit(int status);
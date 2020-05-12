#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#define BUFLENGHT 256

typedef enum {IWANT, RECVD, ENTER, IAMIN, TIMUP, TOOLATE, CLOSD, FAILD, GAVUP} operation;

struct server_args {
    unsigned int nsecs;
    int nplaces;
    int nthreads;
    char fifoname[BUFLENGHT];
};

struct client_args {
    unsigned int nsecs;
    char fifoname[BUFLENGHT];
};

struct server_args checkServerArgs(int argc, char* argv[]);

struct client_args checkClientArgs(int argc, char* argv[]);

void initializeTime();

void toStringOperation(operation oper, char res[]);

void display(int i, int pid, long tid, int duration, int place, operation oper);
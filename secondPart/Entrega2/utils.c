#include "utils.h"

long initialTime;

void initializeTime(){
    initialTime = time(NULL);
}

struct server_args checkServerArgs(int argc, char* argv[]) {
    if (argc < 4 || argc > 8){
        printf("Usage: Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
        exit(1);
    }
    struct server_args args = {0, 0, 0, ""};

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') { //- options
            if (!strcmp(argv[i], "-t") && atoi(argv[++i]) > 0) { 
                int nsecs = atoi(argv[i]);
                args.nsecs = nsecs;
            } else if (!strcmp(argv[i], "-l") && atoi(argv[++i]) > 0) {
                int nplaces = atoi(argv[i]);
                args.nplaces = nplaces;
            } else if (!strcmp(argv[i], "-n") && atoi(argv[++i]) > 0) {
                int nthreads = atoi(argv[i]);
                args.nthreads = nthreads;
            } else {
                printf("Usage: Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
                exit(1);
            }
        } else if (args.fifoname[0] == '\0') { //not a -option and no fifoname provided
            if (strlen(argv[i]) > BUFLENGHT) {
                printf("Usage: Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
                exit(1);
            }
            strcpy(args.fifoname, argv[i]);
        } else { //fifoname is not empty and another one was provided
            printf("Usage: Q2 <-t nsecs> [-l nplaces] [-n nsecs] fifoname\n");
            exit(1);
        }
    }
    return args;
}

struct client_args checkClientArgs(int argc, char* argv[]) {
    if (argc != 4){
        printf("Usage: U2 <-t nsecs> fifoname\n");
        exit(1);
    }
    struct client_args args = {0, ""};

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') { //- options
            if (!strcmp(argv[i], "-t") && atoi(argv[++i]) > 0) {
                int nsecs = atoi(argv[i]);
                args.nsecs = nsecs;
            } else {
                    printf("Usage: U2 <-t nsecs> fifoname\n");
                    exit(1);
            }
        } else if (args.fifoname[0] == '\0') { //if string is empty it contains \0
            if (strlen(argv[i]) > BUFLENGHT) {
                printf("Usage: U2 <-t nsecs> fifoname\n");
                exit(1);
            }
            strcpy(args.fifoname, argv[i]);
        } else { //fifoname is not empty and another one was provided
            printf("Usage: U2 <-t nsecs> fifoname\n");
            exit(1);
        }
    }
    return args;
}

void toStringOperation(operation oper, char res[]){
    switch(oper){
        case IWANT:
            strcpy(res, "IWANT");
            break;
        case RECVD:
            strcpy(res, "RECVD");
            break;
        case ENTER:
            strcpy(res, "ENTER");
            break;
        case IAMIN:
            strcpy(res, "IAMIN");
            break;
        case TIMUP:
            strcpy(res, "TIMUP");
            break;
        case TOOLATE:
            strcpy(res, "2LATE");
            break;
        case CLOSD:
            strcpy(res, "CLOSD");
            break;
        case FAILD:
            strcpy(res, "FAILD");
            break;
        case GAVUP:
            strcpy(res, "GAVUP");
            break;
        default: break;
    }

}

void display(int i, int pid, long tid, int duration, int place, operation oper){
    char stringOperation[BUFLENGHT];
    long curr_time = time(NULL) - initialTime;
    toStringOperation(oper, stringOperation);
    fprintf(stdout, "%d ; %d ; %d ; %ld ; %d ; %d ; %s\n", (int) (curr_time) ,i, pid, tid, duration, place, stringOperation);
    setbuf(stdout, NULL);
}

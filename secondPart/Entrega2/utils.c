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
        char* arg = argv[i];

        if (arg[0] == '-') { //- options
            if (strcmp(arg, "-t") == 0) { 
                i++;
                int nsecs = atoi(argv[i]);
                if (nsecs <= 0) { //nsecs must be positive
                    printf("Usage: Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
                    exit(1);
                } else {
                    args.nsecs = nsecs;
                }
            } 
            else if (strcmp(arg, "-l")) {
                i++;
                int nplaces = atoi(argv[i]);
                if (nplaces <= 0) { //nplaces must be positive
                    printf("Usage: Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
                    exit(1);
                } else {
                    args.nplaces = nplaces;
                }
            }
            else if (strcmp(arg, "-n")) {
                i++;
                int nthreads = atoi(argv[i]);
                if (nthreads <= 0) { //nthreads must be positive
                    printf("Usage: Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
                    exit(1);
                } else {
                    args.nthreads = nthreads;
                }
            } else { //no other option besides -t, -l and -n
                printf("Usage: Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
                exit(1);
            } 

        } else if (args.fifoname[0] == '\0') { //not a -option and no fifoname provided
            if (strlen(arg) > BUFLENGHT) {
                printf("Usage: Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
                exit(1);
            }
            strcpy(args.fifoname, arg);
        } /*else { //fifoname already provided
            printf("Usage: Q2 <-t nsecs> [-l nplaces] [-n nsecs] fifoname\n");
            exit(1);
        }*/
    }

    return args;
}

struct client_args checkClientArgs(int argc, char* argv[]) {
    if (argc != 4){
        printf("Usage: U2 <-t nsecs> fifoname\n");
        exit(1);
    }
    struct client_args args;

    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];

        if (arg[0] == '-') { //- options
            if (strcmp(arg, "-t") == 0) { 
                i++;
                int nsecs = atoi(argv[i]);
                if (nsecs <= 0) { //nsecs must be positive
                    printf("Usage: U1 <-t nsecs> fifoname\n");
                    exit(1);
                } else {
                    args.nsecs = nsecs;
                }
            } else { //no other option besides -t
                printf("Usage: U1 <-t nsecs> fifoname\n");
                exit(1);
            } 
        } else if (args.fifoname[0] == '\0') { //not a -option and no fifoname provided
            if (strlen(arg) > BUFLENGHT) {
                printf("Usage: U1 <-t nsecs> fifoname\n");
                exit(1);
            }
            strcpy(args.fifoname, arg);
        } else { //fifoname already provided
            printf("Usage: U1 <-t nsecs> fifoname\n");
            exit(1);
        }
    }
    return args;
}

void toStringOperation(operation oper, char res[]){
    //char res[BUFLENGHT];
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


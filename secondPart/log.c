#include "log.h"

long initialTime;

void initializeTime(){
    initialTime = time(NULL);
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
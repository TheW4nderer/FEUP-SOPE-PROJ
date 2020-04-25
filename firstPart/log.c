#include "log.h"

FILE* log_file;

void startLog(){
    setenv("LOG_FILENAME","log.txt",0);
    
    log_file = fopen(getenv("LOG_FILENAME"),"a");

    if (log_file == NULL){
        perror("Error opening/creating file");
        exit(1);
    }
}

void writeToFile(Log *log){
    char* action;
    switch (log->action){
        case CREATE: action = "CREATE"; break;
        case EXIT: action = "EXIT"; break;
        case RECV_SIGNAL: action = "RECV_SIGNAL"; break;
        case SEND_SIGNAL: action = "SEND_SIGNAL"; break;
        case RECV_PIPE: action = "RECV_PIPE"; break;
        case SEND_PIPE: action = "SEND_PIPE"; break;
        case ENTRY: action = "ENTRY"; break;
    }
    fprintf(log_file, "%-5.2f - %-5d - %-11s - %s\n", log->instant, log->pid, action, log->info);
    setbuf(log_file,NULL);
}

void createLog(Action action, Log* log){
    clock_t current_time = clock();
    log->instant = (current_time)/(CLOCKS_PER_SEC / (double) 1000);
    log->pid = getpid();
    log->action = action;
    strncpy(log->info,"Command: ",sizeof("Command: "));
}

void regCreate(int argc, char* argv[]){
    Log log;
    createLog(CREATE, &log);
    for (int i = 0; i < argc; i++){
        strcat(log.info,argv[i]);
        if (i != argc-1){
            strcat(log.info," ");
        }
    }
    writeToFile(&log);
}


void regExit(int status){
    Log log;
    createLog(EXIT, &log);
    sprintf(log.info, "Exit Status: %d",status);
    writeToFile(&log);
    exit(status);
}


void regRecvSignal(int signal){
    Log log;
    createLog(RECV_SIGNAL, &log);
    sprintf(log.info, "Received Signal: %d",signal);
    writeToFile(&log);
}


void regSendSignal(int signal, pid_t pid){
    Log log;
    createLog(SEND_SIGNAL, &log);
    sprintf(log.info, "Sent Signal %d to Process %d",signal,pid);
    writeToFile(&log);
}


void regRecvPipe(int msg){
    Log log;
    createLog(RECV_PIPE, &log);
    sprintf(log.info, "Received Size: %d",msg);
    writeToFile(&log);
}


void regSendPipe(int msg){
    Log log;
    createLog(SEND_PIPE, &log);
    sprintf(log.info,"Sent Size: %d",msg);
    writeToFile(&log);
}


void regEntry(int size, char* path){
    Log log;
    createLog(ENTRY, &log);
    sprintf(log.info,"Size: %d\tPath: %s", (int) size, path);
    writeToFile(&log);
}

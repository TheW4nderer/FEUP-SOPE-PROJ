#include "log.h"

FILE* log_file;
clock_t start_time;

void startLog(){
    start_time = clock();
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
    fprintf(log_file, "%.2f - %d - %s - %s\n", log->instant, log->pid, action, log->info);
}

void createLog(Action action, Log* log){
    clock_t current_time = clock();
    //printf("%f\n",(current_time)/(CLOCKS_PER_SEC / (double) 1000));
    log->instant = (current_time - start_time)/(CLOCKS_PER_SEC / (double) 1000);
    log->pid = getpid();
    log->action = action;
    strncpy(log->info,"",sizeof(""));
}

void regCreate(int argc, char* argv[]){
    Log log;
    createLog(CREATE, &log);
    for (int i = 0; i < argc; i++){
        strcat(log.info,argv[i]);
        if (i != argc-1){
            //strcat(log.info,argv[i]);
            strcat(log.info," ");
        }
        /*else{
            strcat(log.info,argv[i]);
        }*/
    }
    writeToFile(&log);
}

void regExit(int status){
    Log log;
    createLog(EXIT, &log);
    sprintf(log.info, "%d",status);
    writeToFile(&log);
    exit(status);
}
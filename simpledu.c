#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include <signal.h>

#include "args.h"
#include "log.h"
#define MAX_COMMANDS 10
#define MAX_SUBDIRS 256
#define READ 0 
#define WRITE 1 

struct Args args = {0,0,1024,0,0,0,-1, 0};



int ceiling(double num) {
    int inum = (int)num;
    if (num == (float)inum) {
        return inum;
    }
    return inum + 1;
}


void getNumber(char* cmd){
    char* blockSize = "--block-size=";
    char* max_d = "--max-depth=";
    if (strncmp(cmd, blockSize,13) == 0){
        cmd = strtok(cmd, "=");
        args.block_size = atoi(strtok(NULL, "="));
    }

    else if (strncmp(cmd, max_d, 12) == 0){
        cmd = strtok(cmd, "=");
        args.max_depth = atoi(strtok(NULL, "="));
    }
}


//checks the command array for the flags to be activated
void checkArgumensArray(char* argv[MAX_COMMANDS], int numArgs) {
    char* all1 = "-a";
    char* all2 = "--all";
    char* bytes1 = "-b";
    char* bytes2 = "--bytes";
    char* block1 = "-B";
    char* count_links1 = "-l";
    char* count_links2 = "--count-links";
    char* dereference1 = "-L";
    char* dereference2 = "--dereference";
    char* separate1 = "-S";
    char* separate2 = "--separate-dirs";

    for (int i = 0; i < numArgs; i++){
        if (strcmp(all1, argv[i]) == 0 || strcmp(all2, argv[i]) == 0) args.all = 1;

        else if (strcmp(bytes1, argv[i]) == 0 || strcmp(bytes2, argv[i]) == 0) args.bytes = 1;

        else if (strcmp(block1, argv[i]) == 0){
            args.block_size = atoi(argv[i+1]); 
            args.block_size_changed = 1;
        }

        else if (strcmp(count_links1, argv[i]) == 0 || strcmp(count_links2, argv[i]) == 0) args.count_links = 1;

        else if (strcmp(dereference1, argv[i]) == 0 || strcmp(dereference2, argv[i]) == 0) args.dereference = 1;

        else if (strcmp(separate1, argv[i]) == 0 || strcmp(separate2, argv[i]) == 0) args.separate_dirs = 1;

        else getNumber(argv[i]);

    }
}

int countChars(char* path){
    int lenght = strlen(path);
    int count = 0;
    for (int i = 0; i < lenght; i++){
        if (path[i] == '/') count++;
    }
    return count;
}


int checkValidPath(char* originalPath, char* current_path){
    int numSubdirs = countChars(current_path) - countChars(originalPath);
    if (args.max_depth != -1){
        if (numSubdirs > args.max_depth) return 0;
    }
    return 1;
}


void showRegInfo(char* path){
    struct stat stat_buf;
    if (!args.dereference)
        lstat(path, &stat_buf);
    else stat(path, &stat_buf);
    
    if (S_ISREG(stat_buf.st_mode) || S_ISLNK(stat_buf.st_mode)){
        if (args.block_size_changed){
            double res =  (( (stat_buf.st_blocks/2) * 1024.0 / (double)args.block_size));
            res = ceiling(res);
            int r = res;
            printf("%d\t%s\n",r, path);
        }    
        else if (args.bytes){
            printf("%ld\t%s\n", stat_buf.st_size, path);
        }
        else    
            printf("%ld\t%s\n", stat_buf.st_blocks/2, path);
    
    }
}

void sigint_handler(int signo){
    int pid = getpid();
    char terminate;
    printf("Entering SIGINT handler\n");
    kill(-2, SIGTSTP);
    printf("Do you want to terminate? (y/n)");
    scanf("%c", &terminate);

    if (terminate == 'y') kill(pid, SIGTERM);

    else if (terminate == 'n') kill(-2, SIGCONT);

    else printf("Invalid!\n");

    printf("Exiting SIGINT handler\n");
    
}



int getDirSize(char* path, char* original){
    DIR* dir;
    struct dirent* dirp;
    struct stat stat_buf, curr_dir, original_file;
    char newpath[BUFFER_SIZE];
    int result = 0;
    pid_t pid;
    if (!args.dereference)
        lstat(path, &curr_dir);
    else stat(path, &curr_dir);

    if (!args.dereference)
        lstat(original, &original_file);
    else stat(original, &original_file);

    if (S_ISREG(original_file.st_mode) || S_ISLNK(original_file.st_mode)){
        showRegInfo(original);
        return 0;
    }
    /*
    struct sigaction action;
    action.sa_handler = sigint_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    
    if (sigaction(SIGINT, &action, NULL) < 0){
        fprintf(stderr, "Unable to install handler\n");
        exit(1);
    }
    */

    if ((dir = opendir(path)) == NULL){
        perror(path);
        exit(1);
    }

    while ((dirp = readdir(dir)) != NULL){
        strcpy(newpath, path);
        strcat(newpath, "/");
        strcat(newpath, dirp->d_name);
        pid_t status;
        if (!args.dereference)
            lstat(newpath, &stat_buf); //considerando a flag -L ativa
        else stat(newpath, &stat_buf);
        if (S_ISDIR(stat_buf.st_mode) && strcmp(".", dirp->d_name) && strcmp("..", dirp->d_name)){
            
            int fd[2];
            pipe(fd);
            pid = fork();
            if (pid == 0){      //Processo-Filho
                int size = getDirSize(newpath, original);
                if (args.separate_dirs) size = 0;
                close(fd[READ]);
                write(fd[WRITE], &size, sizeof(int)); 
                close(fd[WRITE]);
                exit(0);
            }
            else if (pid > 0){  //Processo-Pai
                waitpid(-1, &status, 0);
                //regExit(status);
                int size_received;
                close(fd[WRITE]);
                read(fd[READ], &size_received, sizeof(int));
                result += size_received;
            }
        }
        else if (S_ISREG(stat_buf.st_mode) || S_ISLNK(stat_buf.st_mode))
        {
            if (args.all)
                if (checkValidPath(original, newpath)) showRegInfo(newpath);

            if (args.block_size_changed){
                result += stat_buf.st_blocks/2;
            }
            if (args.bytes){
                result += stat_buf.st_size;
            }
            else if (!args.bytes && !args.block_size_changed)
                result += (stat_buf.st_blocks/2);
        }
    }    
    double res = 0;
    if (args.bytes){
        result += curr_dir.st_size;
        res = result;
    }
    else if (args.block_size_changed){
        result += curr_dir.st_blocks/2;
        res = result * 1024 / (double) args.block_size;
        res = (double) ceiling(res);
    }
    else{
        result += curr_dir.st_blocks/2;
        res = result;
    }    
    if (checkValidPath(original, path))    
        printf("%d\t%s\n", (int) res, path);

    return result;
}



int main(int argc, char* argv[], char* envp[]){
    startLog();
    regCreate(argc, argv);
    if (argc < 2){
        printf("Usage: du -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n");
        exit(1);
    }

    checkArgumensArray(argv, argc);
    getDirSize(argv[2], argv[2]);
    
    regExit(0);
    

    return 0;
}
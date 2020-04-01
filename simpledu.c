#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <math.h>
#include "args.h"
#define MAX_COMMANDS 10

struct Args args = {0,0,1024,0,0,0,-1};



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
            args.bytes = 1;
        }

        else if (strcmp(count_links1, argv[i]) == 0 || strcmp(count_links2, argv[i]) == 0) args.count_links = 1;

        else if (strcmp(dereference1, argv[i]) == 0 || strcmp(dereference2, argv[i]) == 0) args.dereference = 1;

        else if (strcmp(separate1, argv[i]) == 0 || strcmp(separate2, argv[i]) == 0) args.separate_dirs = 1;

        else getNumber(argv[i]);

    }

}

void showRegInfo(char* path){
    struct stat stat_buf;
    lstat(path, &stat_buf);
    


    if (S_ISREG(stat_buf.st_mode)){
        if (args.bytes){
            double res =  (( (stat_buf.st_blocks/2) * 1024.0 / (double)args.block_size));
            res = ceil(res);
            int r = res;

            printf("%d      %s\n",r, path);
            //printf("%d\n", args.block_size);
        }    
        else    
            printf("%ld     %s\n", stat_buf.st_blocks/2, path);
    
    }
}


void getDirInfo(char* path){
    DIR* dir;
    struct dirent* dirp;
    struct stat stat_buf;
    char newpath[BUFFER_SIZE];
    //pid_t pid;

    if ((dir = opendir(path)) == NULL){
        perror(path);
        exit(1);
    }

    while ((dirp = readdir(dir)) != NULL){
        strcpy(newpath, path);
        strcat(newpath, "/");
        strcat(newpath, dirp->d_name);
        lstat(newpath, &stat_buf); //considerando a flag -L ativa
        showRegInfo(newpath);
        
    }
}


int getDirSize(char* path){
    DIR* dir;
    struct dirent* dirp;
    struct stat stat_buf, curr_dir;
    char newpath[BUFFER_SIZE];
    int result = 0;
    lstat(path, &curr_dir);

    if ((dir = opendir(path)) == NULL){
        perror(path);
        exit(1);
    }

    while ((dirp = readdir(dir)) != NULL){
        strcpy(newpath, path);
        strcat(newpath, "/");
        strcat(newpath, dirp->d_name);
        lstat(newpath, &stat_buf); //considerando a flag -L ativa
        result += (stat_buf.st_blocks/2);
    }    

    if (args.bytes)
        return ((result - curr_dir.st_blocks/2)*1024)/args.block_size;
    return result - curr_dir.st_blocks/2;
}



int searchDir(char* path){
    struct stat stat_buf;
    if (args.all) getDirInfo(path);
    lstat(path, &stat_buf);
    int size = getDirSize(path);

    printf("%d         %s\n", size, path);
    
    return 0;
    
}


int main(int argc, char* argv[], char* envp[]){

    //int fd;
    if (argc < 2){
        printf("Usage: du -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n");
        exit(1);
    }

    
    checkArgumensArray(argv, argc);

    //fd = open("out.txt", O_WRONLY | O_TRUNC | O_SYNC, 0600);
    //if (fd == -1) printf("Error writing in file\n");

    //dup2(fd, STDOUT_FILENO);

    searchDir(argv[2]);

    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
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
    char* block2 = "--block-size=";
    char* count_links1 = "-l";
    char* count_links2 = "--count-links";
    char* dereference1 = "-L";
    char* dereference2 = "--dereference";
    char* separate1 = "-S";
    char* separate2 = "--separate-dirs";

    for (int i = 0; i < numArgs; i++){
        if (strcmp(all1, argv[i]) == 0 || strcmp(all2, argv[i]) == 0) args.all = 1;

        else if (strcmp(bytes1, argv[i]) == 0 || strcmp(bytes2, argv[i]) == 0) args.bytes = 1;

        else if (strcmp(block1, argv[i]) == 0 || strcmp(block2, argv[i]) == 0); //change block size

        else if (strcmp(count_links1, argv[i]) == 0 || strcmp(count_links2, argv[i]) == 0) args.count_links = 1;

        else if (strcmp(dereference1, argv[i]) == 0 || strcmp(dereference2, argv[i]) == 0) args.dereference = 1;

        else if (strcmp(separate1, argv[i]) == 0 || strcmp(separate2, argv[i]) == 0) args.separate_dirs = 1;

        else getNumber(argv[i]);

    }

}

void showRegInfo(char* path){
    struct stat stat_buf;
    stat(path, &stat_buf);

    if (S_ISREG(stat_buf.st_mode)){
        printf("Total size (bytes): %ld\n", stat_buf.st_size);
        printf("Total size (blocks of %ld): %ld\n", stat_buf.st_blksize, stat_buf.st_blocks);
    }
}


int searchDir(char* path){
    DIR* dir;
    struct dirent *dirp;
    char newpath[BUFFER_SIZE];
    if ((dir = opendir(path)) == NULL){
        perror(path);
        exit(1);
    }

    while ((dirp = readdir(dir)) != NULL){
        strcpy(newpath, path);
        strcat(newpath, "/");
        strcat(newpath, dirp->d_name);
        showRegInfo(newpath);
        printf("\n");
    }
    
    return 0;
    
}


int main(int argc, char* argv[], char* envp[]){

    int fd;
    if (argc < 2){
        printf("Usage: du -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n");
        exit(1);
    }


    //args.path = argv[2];
    //struct stat stat_buf;

    //stat(argv[2], &stat_buf); //ignoring symbolic links
    
    checkArgumensArray(argv, argc);

    fd = open("out.txt", O_WRONLY | O_TRUNC | O_SYNC, 0600);
    if (fd == -1) printf("Error writing in file\n");

    dup2(fd, STDOUT_FILENO);

    searchDir(argv[2]);

    //checkArgumensArray(argv, argc,0);
    //printf("ARGS = {%d, %d, %d, %d, %d, %d, %d}\n", args.all, args.bytes, args.block_size, args.count_links, args.dereference, args.separate_dirs, args.max_depth);


    return 0;
}
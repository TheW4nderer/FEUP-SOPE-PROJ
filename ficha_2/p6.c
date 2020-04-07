#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <dirent.h> 
#include <sys/stat.h> 
#include <errno.h> 

int main(int argc, char *argv[]) { 
    DIR *dirp;  
    struct dirent *direntp;  
    struct stat stat_buf;  
    char *str;
    unsigned long inode, size;
    if (argc != 2)  {
           fprintf( stderr, "Usage: %s dir_name\n", argv[0]);   
           exit(1);  
        }  
        if ((dirp = opendir( argv[1])) == NULL)  {
            perror(argv[1]);   
            exit(2);  
        }  
        while ((direntp = readdir( dirp)) != NULL)  {
            lstat(direntp->d_name, &stat_buf);
            if (S_ISREG(stat_buf.st_mode)){ 
                str = "regular";
                inode = stat_buf.st_ino;
                size = stat_buf.st_size;

            }
            else if (S_ISDIR(stat_buf.st_mode)) {
                str = "directory";
                inode = stat_buf.st_ino;
                size = stat_buf.st_size;

            }
            else str = "other";
        
            printf("%-25s - %-10s - i-node - %ld - \t size - %ld\n", direntp->d_name, str, inode, size);
        }
    closedir(dirp);
    exit(0); 
}
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char* argv[]){
    char* dir = (argc==2) ? argv[1] : ".";
    DIR* d = opendir(dir);
    struct dirent* de = NULL;
    while(de = readdir(d)){

        struct stat info;
        char name[FILENAME_MAX] = {};
        snprintf(name, FILENAME_MAX,"%s/%s", dir, de->d_name);
        stat(name, &info);
        S_ISREG(info.st_mode);
        printf("%s ", de->d_name);
        int res = (info.st_mode & S_IFMT);
        switch(res){
            case S_IFREG:
                printf("file");
                break;
            case S_IFDIR:
                printf("directory");
                break;
            case S_IFLNK:
                printf("symbolic link");
                break;
            case S_IFCHR:
                printf("character device");
            case S_IFBLK:
                printf("block device");
            case S_IFIFO:
                printf("FIFO");
        }
        printf("\n");
    }
}
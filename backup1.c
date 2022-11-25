#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#include <unistd.h>

#include <errno.h>
#include <string.h>

#include "logger.c"

const int BUF_SIZE = 4096;

int copy(char* pathIn, char* pathOut){
    int fIn = open(pathIn, O_RDONLY);
    if (fIn == -1) return 1;

    int fOut = open(pathOut, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fOut == -1) return 2;

    char buf[BUF_SIZE];
    int rBytes = 0, wBytes = 0;

    while ((rBytes = read(fIn, &buf, sizeof(buf))) > 0){
        wBytes = write(fOut, &buf, rBytes);
        if (wBytes != rBytes) return wBytes - rBytes;
    }

    close(fIn);
    close(fOut);
    return 0;
}

int cmp(char * path1, char * path2){
    char buf1[BUF_SIZE];
    memset(buf1, 0, sizeof(buf1));

    char buf2[BUF_SIZE];
    memset(buf2, 0, sizeof(buf2));

    int fd1 = open(path1, O_RDONLY);
    if (fd1 == -1) return -1;

    int fd2 = open(path2, O_RDONLY);
    if (fd2 == -1) return -2;

    int rBytes1, rBytes2;

    while ((rBytes1 = read(fd1, &buf1, sizeof(buf1))) > 0  &&
           (rBytes2 = read(fd2, &buf2, sizeof(buf2))) > 0){
        if (memcmp(buf1, buf2, BUF_SIZE) != 0 || rBytes1 != rBytes2)
            return 1;
    }

    close(fd1);
    close(fd2);
    return 0;
}

void cp(char* orig, char* dest){
    pid_t pid = fork();
    if (pid == 0){
        execlp("cp", "cp", orig, dest, NULL);
        logger("Error exec cp for: ", orig, "->", dest, " ", strerror(errno), NULL);
        exit(1);
    }
    wait(NULL);
    return;
}

void gzip(char* filepath){
    pid_t pid = fork();
    if (pid == 0){
        execlp("gzip", "gzip", "-f", filepath, NULL);
        logger("Error exec gzip for: ", filepath, " ", strerror(errno), NULL);
        exit(1);
    }
    wait(NULL);
    return;
}

int getStat(char* dir, char* name, struct stat* info, int gz){
    char path[FILENAME_MAX];
    if (gz == 1)
        sprintf(path, "%s/%s.gz", dir, name);
    else
        sprintf(path, "%s/%s", dir, name);

    return stat(path, info);
}


void backup(char* srcPath, char* destPath){
    DIR* src; DIR* dest;
    src = opendir(srcPath);
    dest = opendir(destPath);

    if(src == NULL){
        logger("Error open src dir: ", srcPath," ", strerror(errno), NULL);
        return;
    }

    if(dest == NULL){
        if (mkdir(destPath, 0777) == -1) {
            logger("Error mkdir: ", destPath, " ", strerror(errno), NULL);
            return;
        }
        dest = opendir(destPath);
        if (dest == NULL){
            logger("Error open dest dir: ", destPath, " ", strerror(errno), NULL);
            return;
        }
    }
    struct dirent* dirent;
    while((dirent = readdir(src)) != NULL){
        if(strcmp(".", dirent->d_name) == 0 || strcmp("..", dirent->d_name) == 0) continue;

        struct stat srcStat;
        if(getStat(srcPath, dirent->d_name, &srcStat, 0) == -1){
            logger("Error src stat: ", srcPath, "/", dirent->d_name, " ", strerror(errno), NULL);
            return;
        }

        char subSrcPath[FILENAME_MAX];
        char subDestPath[FILENAME_MAX];
        sprintf(subSrcPath, "%s/%s", srcPath, dirent->d_name);
        sprintf(subDestPath, "%s/%s", destPath, dirent->d_name);

        int type = srcStat.st_mode & S_IFMT;
        if(type == S_IFDIR){
            backup(subSrcPath, subDestPath);
        }
        else if(type == S_IFREG){
            struct stat destStat;
            if(getStat(destPath, dirent->d_name, &destStat, 1) == 0){
                if(srcStat.st_mtime > destStat.st_mtime){
                    cp(subSrcPath, destPath);
                    gzip(subDestPath);
                }
            }
            else if(errno == ENOENT){
                cp(subSrcPath, destPath);
                gzip(subDestPath);
            }
            else{
                logger("Error dest stat", strerror(errno), NULL);
                return;
            }
        }
    }
    closedir(src);
    closedir(dest);
}

int main(int argc, char* argv[]){
    if (argc != 3) {
        logger("Wrong number of arguments", NULL);
        return 1;
    }

    backup(argv[1], argv[2]);

    return 0;
}

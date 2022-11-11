#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void logger(char* str, ...){
    va_list valist;
    va_start(valist, str);

    while (str != NULL) {
        printf("%s", str);
        str = va_arg(valist, char*);
    }
    printf("\n");

    va_end(valist);
}


int main(int argc, char** argv){
    if (argc != 2){
        logger("Wrong number of arguments", NULL);
        return 1;
    }

    FILE *stream = fopen(argv[1], "r");
    if (stream == NULL){
        logger("Wrong file", NULL);
        return 1;
    }

    char str[FILENAME_MAX] = {};
    int time = 0;
    int scanReturn = 0;

    while ((scanReturn = fscanf(stream, "%d %s", &time, str)) == 2){
        if (time < 0){
            logger("Wrong delay time", NULL);
            continue;
        }

        pid_t pid = fork();
        if (pid == -1){
            logger("Fork error: ", strerror(errno), NULL);
        }else if (pid == 0){
            logger(str, " is running in child", NULL);
            sleep(time);
            execlp(str, str, NULL);
            logger("Exec error: ", strerror(errno), NULL);
            break;
        }
    }

    fclose(stream);
    return 0;
}
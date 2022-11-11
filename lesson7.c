#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wait.h>

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
/*
pid_t waitpid(pid_t pid, int* status, int opts); ошибка == -1
                                            ^
                                        WNOHANG - не ждать

void handler(int sig){
    while(waitpid(-1, status, WNOHANG > 0){
    }
}
signal(SIGCHLD, handler)


WIFEXITED(status) сам == 1
rc=WEXITSTATUS(status)
WIFSIGNALED(status) убит по сигналу
sig=WTERMSIG(status)
WCOREDUMP(status)

 кор-файлы/дамп (дебаг)
1) $ ulimit -c 999999
2) core files path (google)
3) $ gcc -g src.c
4) $ gdb ./a.out corefile

*/

void handler(int sig){
    while(1) {
        printf("AHAHAHAHHAHAHAHAAHHA");
    }
}

int main(int argc, char** argv){
    signal(SIGINT, handler);
    signal(SIGQUIT, handler);
    while(1);
}
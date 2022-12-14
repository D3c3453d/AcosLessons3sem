#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void problem(char* text) {
    printf("%s\n", text);
    exit(EXIT_FAILURE);
}

pid_t execution(char *input) {
    char *cmdList[FILENAME_MAX];
    int i = 0;
    while((*(cmdList+i) = strsep(&input," ")) != NULL) ++i;

    pid_t pid;
    if ((pid = fork()) == -1) {
        fprintf(stdout, "Fork failure.\n");
        return -1;
    }
    else if (pid == 0) {
        execvp(cmdList[0], cmdList);
        fprintf(stdout, "Command running failure.\n");
        exit(EXIT_FAILURE);
    }
    return pid;
}

int curProcessAmount = 0;

void handler(int sig) {
    int status;
    if (waitpid(-1, &status, 0) < 0) {
        printf("Waitpid error\n");
    }
    else {
        if (WIFEXITED(status)) {
            printf("exited, status=%d\n", WEXITSTATUS(status));
            curProcessAmount--;
        } else if (WIFSIGNALED(status)) {
            printf("killed by signal %d\n", WTERMSIG(status));
            curProcessAmount--;
        } else if (WIFSTOPPED(status)) {
            printf("stopped by signal %d\n", WSTOPSIG(status));
        } else if (WIFCONTINUED(status)) {
            printf("continued\n");
        }
    }
}


int main(int argc, char const *argv[]) {
    if(argc != 2) problem("Wrong number of command line arguments.\n");

    int maxProcessAmount = 0;
    if((maxProcessAmount = strtol(argv[1], NULL, 10)) == 0)
        problem("Incorrect command line argument.\n");

    signal(SIGCHLD, handler);

    char cmd[FILENAME_MAX];
    while(1) {
        fgets(cmd, sizeof(cmd), stdin);
        if(feof(stdin)) break;

        cmd[strlen(cmd) - 1] = 0;
        if(curProcessAmount >= maxProcessAmount) {
            printf("Too much commands running, please wait until some exit.\n");
            continue;
        }
        else if(execution(cmd) >= 0) ++curProcessAmount;
    }
    return 0;
}
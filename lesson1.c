#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void first(){printf("Im first\n");}
void second(){printf("Im second\n");}


int main(int argc, char* argv[]){
    //uid_t uid = getuid();
    //pid_t pid = getpid();
    //pid_t ppid = getppid();

    //printf("User id: %d\n", uid);
    //printf("Process id: %d\n", pid);
    //printf("Parrent process id: %d\n", ppid);

    pid_t p = fork();
    if (p == 0){
        pid_t childPid = getpid();
        pid_t childPpid = getppid();

        printf("Hello from child\n");
        printf("Child pid: %d\n", childPid);
        printf("Child ppid: %d\n", childPpid);
    } else if (p > 0) {
        sleep(10);
        pid_t parrentPid = getpid();
        pid_t parrentPpid = getppid();

        printf("Hello from parrent\n");
        printf("Parrent pid: %d\n", parrentPid);
        printf("Parrent ppid: %d\n", parrentPpid);
    }

    //for(int i = 0; i < argc; ++i){
    //	printf("Arg No%d: %s\n", i, argv[i]);
    //}
    return 0;
}

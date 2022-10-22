#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "msg-defs.h"

void logErr(char* prefix, int err){
    printf("%s: %s\n", prefix, strerror(err));
}

int main(int argc, char** argv){
    key_t k = ftok("token", 0);
    if(k < 0){
        logErr("ftok", errno);
        return errno;
    }

    int mq = msgget(k, IPC_CREAT|0660);
    pid_t pid = getpid();
    struct Task task;
    task.mtype = task_mtype;
    task.sender = pid;
    task.a = atof(argv[1]);
    task.op = argv[2][0];
    task.b = atof(argv[3]);
    msgsnd(mq, &task, sizeof(task)-sizeof(task.mtype), 0);
    struct Ans ans;
    msgrcv(mq, &ans, sizeof(ans)-sizeof(ans.mtype), pid, 0);
    printf("Answer: %g\n", ans.result);
    return 0;
}
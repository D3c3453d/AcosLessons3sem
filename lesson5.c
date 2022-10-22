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

void doMath(struct Task* task, struct Ans* ans){
    switch(task->op){
        case '+':
            ans->result = task->a + task->b;
            break;
        case '-':
            ans->result = task->a - task->b;
            break;
        case '*':
            ans->result = task->a * task->b;
            break;
        case '/':
            ans->result = task->a / task->b;
            break;
    }
}

int main(){
    key_t k = ftok("token", 0);
    if(k < 0){
        logErr("ftok", errno);
        return errno;
    }

    int mq = msgget(k, IPC_CREAT | 0660);
    if(mq < 0 ){
        logErr("msgget", errno);
        return errno;
    }

    while(1){
        struct Task task;
        msgrcv(mq, &task, sizeof(task) - sizeof(task.mtype), task_mtype, 0);
        struct Ans ans;
        ans.mtype = task.sender;

        switch(task.op){
            case '+':
                ans.result = task.a + task.b;
                break;
            case '-':
                ans.result = task.a - task.b;
                break;
            case '*':
                ans.result = task.a * task.b;
                break;
            case '/':
                ans.result = task.a / task.b;
                break;
        }
        //doMath(task*, ans*);
        msgsnd(mq, &ans, sizeof(ans) - sizeof(ans.mtype), 0);
        printf("Server proceeded a task from pid %d\n", task.sender);
    }
    return 0;
}
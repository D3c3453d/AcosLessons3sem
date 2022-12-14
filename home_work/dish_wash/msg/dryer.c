#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "msg-defs.h"

enum {
    ERR_NONE = 0,
    ERR_IO,
    ERR_MEM
};

int working_time(char* file_name, char* type) {
    int time;
    char buf[128];
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }
    while(fscanf(file, "%s : %d", buf, &time) != EOF) {
        if (strcmp(type, buf) == 0) {
            fclose(file);
            return time;
        }
    }
    printf("Unknow type %s\n", type);
    fclose(file);
    return -1;
}

struct queue_msg { //washed dishes 
    long mtype;
    char type[4096];
};

struct dryed_msg { //dryed dishes
    long mtype;
};

int main() {
    char* cost_file = "../dryer.in";
    char* queue_file = "../queue.in";

    FILE* queue = fopen(queue_file, "r");
    if (queue == NULL) {
        perror("fopen");
        return ERR_MEM;
    }
    key_t k = ftok("label", 1);
    if (k < 0) {
        perror("ftok");
        return ERR_MEM;
    }

    int qid = msgget(k, IPC_CREAT | 0660); //queue of msgs
    if (qid < 0 ){
        perror("msgget");
        return ERR_MEM;
    }

    struct queue_msg rcv;
    struct dryed_msg snd;
    snd.mtype = 2;
    while(1){
        if (msgrcv(qid, &rcv, sizeof(rcv) - sizeof(long), 1, 0) < 0) { //getting new dish from queue
            perror("msgrcv");
            return ERR_MEM;
        }
        if (strcmp("stop", rcv.type) == 0) { //checking stop msg
            printf("finish drying\n");
            break;
        }
        if (msgsnd(qid, &snd, sizeof(snd) - sizeof(long), 0) < 0) { //clearing table
            perror("msgsnd");
            return ERR_MEM;
        }
        int time = working_time(cost_file, rcv.type);
        if (time < 0)
            continue;
        printf("taken %s\n", rcv.type);
        sleep(time);
        printf("dryed %s\n", rcv.type);
    }
    return ERR_NONE;
}
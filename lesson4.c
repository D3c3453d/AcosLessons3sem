#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>

int c[3] = {0, 0, 0};
int semId = -1;

struct sembuf opA = {0, +1, 0},
        opD = {0, -1, 0};

void logErr(char* prefix, int err){
    printf("%s: %s\n", prefix, strerror(err));
}

void* counter(void* arg){
    int n = *((int*)arg);
    for(int i = 0; i < 1000000; ++i){
        semop(semId, &opD, 1);
        c[n]++;
        c[2]++;
        semop(semId, &opA, 1);
    }
    return NULL;
}

int main(){
    semId = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0600);
    semop(semId, &opA, 1);
    pthread_t thrId;
    int nn[2] = {0, 1};

    int err = pthread_create(&thrId, NULL, counter, nn + 1);
    if (err != 0){
        logErr("pthread create err: ", err);
        return 1;
    }

    counter(nn + 0);

    err = pthread_join(thrId, NULL);
    if (err != 0){
        logErr("pthread join err: ", err);
        return 1;
    }

    printf("%d %d %d\n", c[0], c[1], c[2]);
    return 0;
}
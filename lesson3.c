#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>

void log_error(char* err){
    perror(err);
}

int main(){
    key_t k = ftok("myshm", 0);
    if (k == -1) {
        log_error("Error: ftok()");
        return 1;
    }

    char isnew = 'Y';

    int shmId = shmget(k, 3 * sizeof(int), IPC_EXCL | IPC_CREAT | 0600);
    if (shmId == -1 && errno == EEXIST){
        isnew = 'N';
        shmId = shmget(k, 3 * sizeof(int), 0);
        if (shmId == -1){
            log_error("Error: ftok()");
            return 1;
        }
    }

    int * c = (int*) shmat(shmId, NULL, 0);
    if(isnew = 'Y'){
        for(int i = 0; i < sizeof(c); ++i) c[i] = 0;
    }
    c[0]++;
    c[2]++;
    printf("%d %d %d\n", c[0], c[1], c[2]);

    shmdt(c);
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

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

int main() {
    char* cost_file = "../dryer.in";
    char* table_file = "table";
    //open table file
    FILE* table = fopen(table_file, "r");
    if (table == NULL) {
        perror("fopen");
        return ERR_MEM;
    }
    //creating key via label file
    key_t k = ftok("label", 1);
    if (k < 0) {
        perror("ftok");
        fprintf(table, "%s\n", "stop");
        return ERR_MEM;
    }
    //getting key of existing sem(with key k)
    int sem_queue = semget(k, 1, IPC_CREAT | 0660);
    if (sem_queue < 0) {
        perror("semget");
        return ERR_MEM;
    }
    struct sembuf take = {0, 1, 0};
    key_t k1 = ftok("label", 2);
    if(k1 < 0){
        perror("ftok");
        fprintf(table, "%s\n", "stop");
        return ERR_MEM;
    }
    int sem_table = semget(k1, 1, IPC_CREAT | 0660);
    struct sembuf t_busy = {0, -1, 0};
    struct sembuf t_unbusy = {0, 1, 0};
    char type[4096];
    //number of dryed dishes
    int num = 0;
    while(1) {
        //printf("checking if table file is ready to use\n");
        semop(sem_table, &t_busy, 1);
        //checking table file
        while(fscanf(table, "%s", type) != EOF) {
            //checking stop
            if (strcmp(type, "stop") == 0) {
                printf("finish drying\n");
                fclose(table);
                return ERR_NONE;
            }
            //getting drying time
            int time = working_time(cost_file, type);
            if (time < 0)
                continue;
            //take dish out of table
            if (semop(sem_queue, &take, 1) < 0) {
                perror("semop");
                return ERR_MEM;
            }
            //increasing amount of washed dishes by one
            ++num;
            printf("taken %s\n", type);
            //drying
            sleep(time);
            printf("dried %s\n", type);
        }
        fclose(table);
        table = fopen(table_file, "r");
        //start from nth element in file table
        for (int i = 0; i < num; ++i) {
            fscanf(table, "%s", type);
        }
        semop(sem_table, &t_unbusy, 1);
    }
    printf("finish drying\n");
    fclose(table);
    return ERR_NONE;
}
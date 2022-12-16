#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>

enum {
    ERR_NONE = 0,
    ERR_IO,
    ERR_MEM
};


//Finding working time of type: type in file: file_name
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


//Getting value of table_limit from env
int table_limit() {
    char* table_limit_string = getenv("TABLE_LIMIT");
    if (table_limit_string == NULL)
        return -1;
    char* end;
    int lim;
    lim = strtol(table_limit_string, &end, 0);
    if (end == table_limit_string){
        printf("not number\n");
        return -1;
    }
    return lim;
}

int main() {
    char* cost_file = "../washer.in";
    char* queue_file = "../queue.in";
    char* table_file = "table";
    //getting table_limit
    int TABLE_LIMIT = table_limit();
    if (TABLE_LIMIT < 0) {
        printf("wrong table limit\n");
        return ERR_IO;
    }
    //printf("TABLE_LIMIT = %d\n", TABLE_LIMIT);
    //open table file
    FILE* table = fopen(table_file, "w");
    if (table == NULL) {
        perror("fopen");
        return ERR_MEM;
    }
    //open current queue file
    FILE* queue = fopen(queue_file, "r");
    if (queue == NULL) {
        perror("fopen");
        fprintf(table, "%s\n", "stop");
        return ERR_MEM;
    }
    //creating key using file label
    key_t k = ftok("label", 1);
    if (k < 0) {
        perror("ftok");
        fprintf(table, "%s\n", "stop");
        return ERR_MEM;
    }
    //creating one new sem using key k 
    int sem_queue = semget(k, 1, IPC_CREAT | 0660);
    if (sem_queue < 0) {
        perror("semget");
        fprintf(table, "%s\n", "stop");
        return ERR_MEM;
    }
    //number of sem, sem_op, sem_flag
    struct sembuf fill = {0, TABLE_LIMIT, 0};
    struct sembuf unfill = {0, -TABLE_LIMIT, 0};
    struct sembuf put = {0, -1, 0};


    if (semop(sem_queue, &fill, 1) < 0){
        perror("semop");
        fprintf(table, "%s\n", "stop");
        return ERR_MEM;
    }
    key_t k1 = ftok("label", 2);
    if(k1 < 0){
        perror("ftok");
        fprintf(table, "%s\n", "stop");
        return ERR_MEM;
    }
    int sem_table = semget(k1, 1, IPC_CREAT | 0660);
    struct sembuf t_file = {0, 1, 0};
    struct sembuf t_busy = {0, -1, 0};
    struct sembuf t_unbusy = {0, 1, 0};
    if (semop(sem_table, &t_file, 1) < 0){
        perror("semop");
        fprintf(table, "%s\n", "stop");
        return ERR_MEM;
    }
    char type[4096];
    int number;
    //start reading queue and washing
    while(fscanf(queue, "%s : %d", type, &number) != EOF) {
        //getting time of washing and amount of dishes 
        int time = working_time(cost_file, type);
        if (time < 0)
            continue;
        //washing each dish
        for (int i = 0; i < number; ++i) {
            //printf("start sleeping\n");
            sleep(time);
            //printf("semop result: %d\n", semop(sem_queue, &put, 1));
            //putting washed dish on table
            if (semop(sem_queue, &put, 1) < 0){
                printf("Out of table size\n");
                perror("semop");
                fprintf(table, "%s\n", "stop");
                fflush(table);
                return ERR_MEM;
            }
            printf("washed %s\n", type);
            //writing dish to file table
            semop(sem_table, &t_busy, 1);
            fprintf(table, "%s\n", type);
            fflush(table);
            semop(sem_table, &t_unbusy, 1);
        }
    }
    printf("finish washing\n");
    fprintf(table, "%s\n", "stop");
    fflush(table);
    fclose(table);
    fclose(queue);
    //checking if table is clear
    if (semop(sem_queue, &unfill, 1) < 0){
        perror("semop");
        fprintf(table, "%s\n", "stop");
        return ERR_MEM;
    }
    int tmp;
    wait(&tmp);
    return ERR_NONE;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

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
    char* washing_cost = "../washer.in";
    char* drying_cost = "../dryer.in";
    char* queue_file = "../queue.in";
    int TABLE_LIMIT = table_limit();
    if (TABLE_LIMIT < 0) {
        printf("wrong table limit\n");
        return ERR_IO;
    }
    int ftable[2];      //dishes on the table
    if (pipe(ftable) == -1) {
        perror("pipe");
        return ERR_MEM;
    }
    int fdryed[2];      //amount of dryed dishes
    if (pipe(fdryed) == -1) {
        perror("pipe");
        return ERR_MEM;
    }
    pid_t p = fork();
    if (p > 0) {    //parent - washer
        close(ftable[0]);
        close(fdryed[1]);
        FILE* queue = fopen(queue_file, "r");
        if (queue == NULL) {
            perror("fopen");
            close(ftable[1]);
            close(fdryed[0]);
            return ERR_MEM;
        }
        int counter = 0;    //amount of dishes on the table
        char type[4096] = {0};
        int number;
        while(fscanf(queue, "%s : %d", type, &number) != EOF) {
            int time = working_time(washing_cost, type);
            type[strlen(type)] = ' ';
            if (time < 0)
                continue;
            for (int i = 0; i < number; ++i) {
                sleep(time);
                if (counter >= TABLE_LIMIT) {
                    char buf[2];
                    int bytes = read(fdryed[0], buf, 1);    //waiting dryer to dry smth
                    if (bytes == 0) {
                        printf("dryer gone away\n");
                        return ERR_MEM;
                    }
                    counter--;
                }
                counter++;
                printf("washed %s\n", type);
                if (write(ftable[1], type, strlen(type)) < strlen(type)) {
                    perror("write");
                    return ERR_MEM;
                }
            }
        }
        char stop[] = "stop";
        if(write(ftable[1], stop, sizeof(stop) - 1) < sizeof(stop) - 1) {
            perror("write");
            return ERR_MEM;
        }
        printf("finish washing\n");
        int status;
        wait(&status);
        close(ftable[1]);
        close(fdryed[0]);
    }
    else if (p == 0) {      // child - dryer
        close(ftable[1]);
        close(fdryed[0]);
        char buf[4097];
        int bytes = 0;
        while((bytes = read(ftable[0], buf, 4096)) > 0) {
            buf[bytes] = '\0';
            char* s;
            char cur_type[1024];
            s = strtok(buf, " ");
            while(s != NULL) {
                strcpy(cur_type, s);
                if (strcmp(cur_type, "stop") == 0) {
                    printf("finish drying\n");
                    close(ftable[0]);
                    close(fdryed[1]);
                    return ERR_NONE;
                }
                int time = working_time(drying_cost, s);
                s = strtok(NULL, " ");
                if (write(fdryed[1], "1", 1) < 1) {
                    perror("write");
                    return ERR_MEM;
                }
                if (time < 0)
                    continue;
                printf("taken %s\n", cur_type);
                sleep(time);
                printf("dried %s\n", cur_type);
            }
        }
        printf("finish drying\n");
        close(ftable[0]);
        close(fdryed[1]);
        return ERR_NONE;
    }


    else if (p == -1) {
        perror("fork");
        close(ftable[1]);
        close(ftable[0]);
        close(fdryed[1]);
        close(fdryed[0]);
        return ERR_MEM;
    }
    return ERR_NONE;
}